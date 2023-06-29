/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Corp.
 * * All Rights Reserved$
 * *
 * * File:
 * *     link.c
 * * Purpose:
 * *     Robo2 Link Scan module.
 * *
 * */
#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <fsal/error.h>
#include <sal_time.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <fsal_int/link.h>
#include <fsal_int/port.h>
#include <fsal_int/stg.h>
#include <fsal_int/lag.h>
#ifdef CONFIG_PORT_EXTENDER
#include <fsal_int/extender.h>
#endif /* CONFIG_PORT_EXTENDER */
#include <avng_phy.h>
/*
 * Variable:
 *  robo_link_control
 * Purpose:
 *  Hold per-unit global status for linkscan
 */

static cbx_ls_cntl_t       cbx_robo2_link_control;

#define LINK_CNTL          cbx_robo2_link_control

/*
 * Define:
 *  LC_LOCK/LC_UNLOCK
 * Purpose:
 *  Serialization Macros for access to lc_cntl structure.
 */

#define LC_LOCK \
        sal_mutex_take(LINK_CNTL.lc_lock, sal_mutex_FOREVER)

#define LC_UNLOCK \
        sal_mutex_give(LINK_CNTL.lc_lock)

#define LC_CHECK_INIT \
          if (!LINK_CNTL.init) {return CBX_E_INIT;}

/*
 * Function:
 *  cbxi_robo2_linkscan_enable_port_get
 * Purpose:
 *  Determine whether or not linkscan is managing a given port
 * Parameters:
 *      unit - Robo2Switch unit #.
 *      port - Port to check.
 * Returns:
 *  CBX_E_NONE - port being scanned
 *  CBX_E_DISABLED - port not being scanned
 */
static int
cbxi_robo2_linkscan_enable_port_get(int unit, cbx_port_t port)
{
    cbx_ls_cntl_t       *ls_cntl;
    cbxi_pgid_t         lpg=CBXI_PGID_INVALID;
    int                 rv;

    LOG_INFO(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API:cbxi_robo2_linkscan_enable_port_get()..\
             port=0x%x\n"),
              port));

    ls_cntl = &LINK_CNTL;

    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(port, &port, &lpg, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LINK,
            (BSL_META("FSAL API : cbxi_robo2_link_scan_enable_port_get()... \
                      Port validation failed for port:%d \n"),port));
        return CBX_E_PORT;
    }

    if (ls_cntl == NULL || ls_cntl->lc_us == 0 ||
            (!CBX_PBMP_MEMBER(ls_cntl->lc_pbm_sw[unit], port))) {
        return (CBX_E_DISABLED);
    }

    return (CBX_E_NONE);
}

/*
 * Function:
 *      cbxi_robo2_link_get
 * Purpose:
 *      Return linkscan's current link status for the given port.
 * Parameters:
 *      unit - RoboSwitch Unit #.
 *      port - RoboSwitch physical port number.
 *      link - (OUT) Current link status.
 * Returns:
 *      CBX_E_NONE - Success
 *      CBX_E_DISABLED - port not being scanned
 * Note:
 *    This routine does not acquire the LC_LOCK, as it only reads a
 *    snapshot of the link bitmaps.  It also cannot hold the LC_LOCK
 *    since it is called indirectly from the linkscan thread
 *    when requesting port info.
 */

int
cbxi_robo2_link_get(int unit, cbx_port_t port, int *link)
{
    cbx_ls_cntl_t   *ls_cntl;

    LC_CHECK_INIT;
    ls_cntl = &LINK_CNTL;

    CBX_IF_ERROR_RETURN(cbxi_robo2_linkscan_enable_port_get(unit, port));

    *link = CBX_PBMP_MEMBER(ls_cntl->lc_pbm_link[unit], port);

    return(CBX_E_NONE);
}
/*
 * Function:
 *      cbxi_robo2_link_scan_update_port
 * Purpose:
 *      Check for and process a link event on one port
 * Parameters:
 *      unit - Robo2Switch unit #.
 *      port - Port to process.
 * Returns:
 *      CBX_E_XXX
 */

static int
cbxi_robo2_link_scan_update_port(int unit, int port)
{
    cbx_ls_cntl_t       *ls_cntl;
    uint32_t            cur_link;
    uint32_t            new_link = 0;
    int                 notify = FALSE;
    cbx_ls_handler_t    *cb_hdlr, *cb_hdlr_next = NULL;
    int                 rv;
    cbx_port_lookup_t   lu;
    cbx_port_t          global_port;
    cbx_portid_t        portid, lagid;
    uint32_t            link_status = 0;
    uint32_t            mac_speed,phy_speed;
#ifdef CONFIG_CASCADED_MODE
    uint32              lag_link_notify = FALSE;
#endif /* CONFIG_CASCADED_MODE */
    cbx_port_params_t port_params;
#ifdef CONFIG_10G_INTF_KR
    uint32 phy_reg;
    uint32_t regdata_rd;
    uint32_t regdata_wr;
    uint16_t link_status_xfi;
    uint16_t pmd_lock;
    uint16_t signal_ok;
#endif

    global_port = port + (unit * CBX_MAX_PORT_PER_UNIT);
    lu.port_index = global_port;
    rv = cbx_port_lookup(&lu, &portid);
    if (rv != CBX_E_NONE)
        return rv;

    ls_cntl = &LINK_CNTL;

#ifdef CONFIG_10G_INTF_KR
    if ((port == 12) || (port == 13)) {
        phy_reg = REG_ACC_TSC_IBLK | (0x3 << 16) | TSCE_RX_X4_STATUS1_PCS_LIVE_STATUS;
        cbx_port_phy_get(portid,0, phy_reg, &regdata_rd);
        link_status_xfi = (regdata_rd & 0x2) >> 1 ;

        phy_reg = REG_ACC_TSC_IBLK | (0x3 << 16) | TSCE_RX_X4_STATUS1_PMA_PMD_LIVE_STATUS;
        cbx_port_phy_get(portid,0, phy_reg, &regdata_rd);
        pmd_lock = (regdata_rd & 0x8) >> 3;
        signal_ok = regdata_rd & 0x1;

        if (link_status_xfi == 0){
            // do nothing if link down already
        } else if ((pmd_lock == 0) || (signal_ok == 0)) {
            //set bit0 = 0 first
            phy_reg = REG_ACC_TSC_IBLK | (0x3 << 16) | TSCE_AN_X4_ABILITIES_ENABLES;
            regdata_wr = TSCE_AN_X4_ABILITIES_ENABLES_AN | TSCE_AN_X4_ABILITIES_ENABLES_CL73;
            cbx_port_phy_set(portid, 0, phy_reg, regdata_wr);

            //set bit0 = 1 to restart AN
            phy_reg = REG_ACC_TSC_IBLK | (0x3 << 16) | TSCE_AN_X4_ABILITIES_ENABLES;
            regdata_wr = TSCE_AN_X4_ABILITIES_ENABLES_AN | TSCE_AN_X4_ABILITIES_ENABLES_CL73 | TSCE_AN_X4_ABILITIES_ENABLES_CL73_AN_RESTART;
            cbx_port_phy_set(portid, 0, phy_reg, regdata_wr);
        }
    }
#endif

    cur_link = CBX_PBMP_MEMBER(ls_cntl->lc_pbm_link[unit], port);

    if (CBX_PBMP_MEMBER(ls_cntl->lc_pbm_sw[unit], port)) {
        rv = cbxi_port_link_status_get(unit, port, &new_link);
    } else {
        return CBX_E_NONE;  /* Port not being scanned */
    }

    if (CBX_FAILURE(rv)) {
        LOG_INFO(BSL_LS_FSAL_LINK,
           (BSL_META("FSAL API : Port %d: Failed to recover link status\n"),
                  port));
        return rv;
    }

    if (cur_link == new_link) {
        return CBX_E_NONE;
    }
    else if (new_link) { /* link up now */
        if ((port==10) || (port==11)) {  /* Only sync up port 10 and 11 of XLMAC */
            CBX_IF_ERROR_RETURN(mac_speed_get(unit,port,(int *)&mac_speed));
            CBX_IF_ERROR_RETURN(avng_phy_speed_get(unit,port,&phy_speed));

            if( phy_speed != mac_speed ) {
               CBX_IF_ERROR_RETURN(mac_speed_set(unit,port,phy_speed));
            }
        }
    }
#if 0    
    else 
    {
        /* Link down */
        /* Go to the default speed */
        if (IS_XL_PORT(unit, port)) {
#if defined(CONFIG_QSGMII) || defined(CONFIG_1000X_1000) || (CONFIG_SGMII_SPEED == 1000)
            mac_speed = 1000;
#endif
            if ((port == 12) || (port == 13)) {
                mac_speed = 10000;
#if (CONFIG_XFI_SPEED == 1000)
                mac_speed = 1000;
#endif
#if (CONFIG_XFI_SPEED == 2500)
                mac_speed = 2500;
#endif
#if defined(CONFIG_10G_INTF_SGMII) || defined(CONFIG_10G_INTF_KX)
                mac_speed = 1000;
#endif
             }
        } else { /* GE ports */
                mac_speed = 1000;
        }
        CBX_IF_ERROR_RETURN(mac_speed_set(unit,port,(int)mac_speed));
    }
#endif
    CBX_IF_ERROR_RETURN(cbx_port_info_get(global_port, &port_params));

    if (new_link) {
        CBX_PBMP_PORT_ADD(ls_cntl->lc_pbm_link[unit], port);
        if (port < 14) {
            rv = cbxi_robo2_lag_member_check(global_port, &lagid);
            if (CBX_SUCCESS(rv)) {
                /* Set forwarding state on lagid */
                CBX_IF_ERROR_RETURN(cbxi_stg_stp_set(CBX_STG_DEFAULT,
                                lagid, cbxStgStpStateForward));


#ifdef CONFIG_PORT_EXTENDER
                if ((cbxi_check_port_extender_mode() == CBX_E_NONE)
                        && SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                    if (port_params.port_type == cbxPortTypeExtenderUpstream) {
                    /* Enable Uplink port in PPFOV if it has been changed to
                     * Avenger Cascade link */
                        cbxi_pe_uplink_failover_set(lagid, new_link);
                    }
                }
#endif /* CONFIG_PORT_EXTENDER */

#ifdef CONFIG_CASCADED_MODE
                lag_link_notify = TRUE;
#endif /* CONFIG_CASCADED_MODE */
            } else {
                CBX_IF_ERROR_RETURN(cbxi_stg_stp_set(CBX_STG_DEFAULT,
                                global_port, cbxStgStpStateForward));

#ifdef CONFIG_PORT_EXTENDER
                if ((cbxi_check_port_extender_mode() == CBX_E_NONE)
                        && SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                    if (port_params.port_type == cbxPortTypeExtenderUpstream) {
                    /* Enable Uplink port in PPFOV if it has been changed to
                     * Avenger Cascade link */
                        cbxi_pe_uplink_failover_set(global_port, new_link);
                    }
                }
#endif /* CONFIG_PORT_EXTENDER */
            }
        }
    } else {
        CBX_PBMP_PORT_REMOVE(ls_cntl->lc_pbm_link[unit], port);
        if (port < 14) {
            rv = cbxi_robo2_lag_member_check(global_port, &lagid);
            if (CBX_SUCCESS(rv)) {
                CBX_IF_ERROR_RETURN(cbx_port_info_get(
                                                lagid, &port_params));

                /* Don't set the port state if  any of the port in LAG is up */
                rv = cbxi_robo2_lag_link_status_get(lagid, &link_status);
                if (link_status == 0) {
#ifdef CONFIG_EXTERNAL_HOST
                    CBX_IF_ERROR_RETURN(cbxi_stg_port_set_all(
                                        lagid, cbxStgStpStateDiscard));

#else
                    if (port_params.port_type != cbxPortTypeExtenderUpstream) {
                        CBX_IF_ERROR_RETURN(cbxi_stg_stp_set(CBX_STG_DEFAULT,
                                        lagid, cbxStgStpStateDiscard));
                    }
#endif

#ifdef CONFIG_PORT_EXTENDER
                    if ((cbxi_check_port_extender_mode() == CBX_E_NONE)
                            && SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                        if (port_params.port_type == cbxPortTypeExtenderUpstream) {
                            rv = cbxi_pe_uplink_failover_set(lagid, new_link);
                            if (CBX_SUCCESS(rv)) {
                            /* Retain cbxStgStpStateForward */
                                CBX_IF_ERROR_RETURN(cbxi_stg_port_set_all(
                                    lagid, cbxStgStpStateForward));
                            }
                        }
                    }
#endif /* CONFIG_PORT_EXTENDER */
                }
#ifdef CONFIG_CASCADED_MODE
                lag_link_notify = TRUE;
#endif /* CONFIG_CASCADED_MODE */
            } else {
                CBX_IF_ERROR_RETURN(cbx_port_info_get(
                                                global_port, &port_params));
#ifdef CONFIG_EXTERNAL_HOST
                CBX_IF_ERROR_RETURN(cbxi_stg_port_set_all(
                                global_port, cbxStgStpStateDiscard));

#else
                if (port_params.port_type != cbxPortTypeExtenderUpstream) {
                    CBX_IF_ERROR_RETURN(cbxi_stg_stp_set(CBX_STG_DEFAULT,
                                         global_port, cbxStgStpStateDiscard));
                }
#endif

#ifdef CONFIG_PORT_EXTENDER
                if ((cbxi_check_port_extender_mode() == CBX_E_NONE)
                        && SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                    if (port_params.port_type == cbxPortTypeExtenderUpstream) {
                        rv = cbxi_pe_uplink_failover_set(global_port, new_link);
                        if (CBX_SUCCESS(rv)) {
                        /* Retain cbxStgStpStateForward */
                            CBX_IF_ERROR_RETURN(cbxi_stg_port_set_all(
                                global_port, cbxStgStpStateForward));
                        }
                    }
                }
#endif /* CONFIG_PORT_EXTENDER */
            }
        }
    }

#ifdef CONFIG_CASCADED_MODE
    if (lag_link_notify) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_lag_link_status_update( global_port,
                                                          new_link, lagid ));
    }
#endif /* CONFIG_CASCADED_MODE */

    /*
     * Call registered handlers with complete link info.
     */

    notify = (cur_link  != new_link);

    LOG_INFO(BSL_LS_FSAL_LINK,
          (BSL_META("FSAL API : Unit %d, Port %d: Link: Current %s, New %s\n"),
              unit, port,
              cur_link ? "up" : "down",
              new_link ? "up" : "down"));

    if (notify) {
        LOG_VERBOSE(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API : Unit %d, Port %d: link notification - %s\n"),
                     unit, port, new_link  ? "up" : "down"));

        for (cb_hdlr = ls_cntl->lc_handler; cb_hdlr; cb_hdlr = cb_hdlr_next) {
            /*
             * save the next linkscan handler first, in case current handler
             * unregister itself inside the handler function
             */
            cb_hdlr_next = cb_hdlr->lh_next;
            cb_hdlr->lh_f(portid, new_link);
        }
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_robo2_link_scan_update
 * Purpose:
 *      Check for a change in link status on each link.  If a change
 *      is detected, call
 *      link, and call the list of registered handlers.
 * Parameters:
 *      unit - Robo2Switch unit #.
 *      pbm - bit map of ports to scan.
 * Returns:
 *      None.
 */

static void
cbxi_robo2_link_scan_update(int unit, pbmp_t pbm)
{
    int                 rv;
    cbx_port_t          port;
    cbxi_pgid_t         lpg=CBXI_PGID_INVALID;
    cbx_port_t          global_port;

    PBMP_ITER(pbm, port) {

        global_port = port + (unit * CBX_MAX_PORT_PER_UNIT);

        /* validate the port index passed */
        rv = cbxi_robo2_port_validate(global_port, &port, &lpg, &unit);
        if (CBX_FAILURE(rv)) {
            LOG_INFO(BSL_LS_FSAL_LINK,
                     (BSL_META("FSAL API : cbxi_robo2_link_scan_update()... \
                          Port validation failed for port:%d \n"),port));
            return;
        }
        if(IS_LOCAL_CPU_PORT(0,port))
            continue;

        rv = cbxi_robo2_link_scan_update_port(unit, port);

        if (CBX_FAILURE(rv)) {
            LOG_INFO(BSL_LS_FSAL_LINK,
                     (BSL_META("FSAL API : cbxi_robo2_link_scan_update()... \
                          Port link scn update failed for port:%d \n"),port));
        }
    }
}

/*
 * Function:
 *  cbxi_robo2_link_scan_pbm_init
 * Purpose:
 *  Initialize the port bitmaps in the link_control structure
 * Parameters:
 *      unit - Robo2Switch unit #.
 * Returns:
 *      None
 */

static void
cbxi_robo2_link_scan_pbm_init(int unit)
{
    cbx_ls_cntl_t       *ls_cntl;

    LOG_INFO(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API : cbxi_robo2_link_scan_pbm_init()..\n")));

    ls_cntl = &LINK_CNTL;

    CBX_PBMP_ASSIGN(ls_cntl->lc_pbm_sw[unit], PBMP_ALL(unit));
    /* keep the link state as down */
    ls_cntl->lc_pbm_link[unit] = 0;
}

/*
 * Function:
 *      cbxi_robo2_linkscan_thread
 * Purpose:
 *      Scan the ports for link status changes and
 *      process them as they occur.
 * Parameters:
 *      None
 * Returns:
 *      None
 */

static void
cbxi_robo2_link_scan_thread()
{
    uint32              num_units = 1;
    cbx_ls_cntl_t       *ls_cntl;
    sal_usecs_t         interval;
    int                 unit = CBX_AVENGER_PRIMARY;

    LOG_INFO(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API : cbxi_robo2_link_scan_thread() \
                      Linkscan starting..\n")));

    /* Check for cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    /* Re-initialize port bitmaps */
    for ( unit = 0; unit < num_units; unit++ ) {
        cbxi_robo2_link_scan_pbm_init(unit);
    }

    ls_cntl = &LINK_CNTL;

    ls_cntl->lc_thread = sal_thread_self();
    ls_cntl->task_status = cbxLinkTaskStatusRunning;

    while ((interval = ls_cntl->lc_us) != 0) {
        for ( unit = 0; unit < num_units; unit++ ) {
            cbxi_robo2_link_scan_update(unit, PBMP_ALL(unit));
        }

        (void)sal_sem_take(ls_cntl->lc_sema, interval);
    }

    LOG_INFO(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API : cbxi_robo2_link_scan_thread() \
                        Linkscan exiting..\n")));

    ls_cntl->lc_thread = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/**************************************************************************
 *                 Link Scan FSAL API IMPLEMENTATION                     *
 **************************************************************************/
/*
 * Function:
 *      cbx_link_scan_cntl_t_init
 * Purpose:
 *      Initialize Link scan control structure parameters.
 * Parameters:
 *      ls_cntl - Pointer to Link scan control structure.
 * Returns:
 *      None
 */

void cbx_link_scan_cntl_t_init( cbx_ls_cntl_t *ls_cntl )
{
    if ( NULL != ls_cntl ) {
        sal_memset( ls_cntl, 0, sizeof ( cbx_ls_cntl_t ));
    }
    return;
}

/**
 * Function:
 *      cbx_link_scan_init
 * Purpose:
 * Initialize Link scan module
 * This routine is used to init link scan.
 *
 * The link_scan thread is created during this call.
 * Link scanning is not performed until enabled using cbx_link_scan_enable_set
 *
 * @param link_scan_params  (IN)  Link scan parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_init( cbx_link_scan_params_t *link_scan_params )
{
    int                 unit = CBX_AVENGER_PRIMARY;
    uint32              num_units = 1;
    cbx_link_scan_params_t ls_params;

    LOG_INFO(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API : cbx_link_scan_init()..\n")));

    if (LINK_CNTL.init == TRUE) {
        ls_params.usec = 0;/* To disable link scan */
        CBX_IF_ERROR_RETURN(cbx_link_scan_detach(&ls_params));
    }

    cbx_link_scan_cntl_t_init(&LINK_CNTL);

    LINK_CNTL.lc_lock = sal_mutex_create("cbx_link_LOCK");
    if (LINK_CNTL.lc_lock == NULL) {
        return CBX_E_MEMORY;
    }

    LINK_CNTL.lc_sema = sal_sem_create("cbx_link_SLEEP",
                                       0, 0);
    if (LINK_CNTL.lc_sema == NULL) {
        sal_mutex_destroy(LINK_CNTL.lc_lock);
        return CBX_E_MEMORY;
    }

    /* Check for cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    /* Re-initialize port bitmaps */
    for ( unit = 0; unit < num_units; unit++ ) {
        cbxi_robo2_link_scan_pbm_init(unit);
    }

    /* Link scan initialization complete */
    LINK_CNTL.init = TRUE;
    LINK_CNTL.task_status = cbxLinkTaskStatusHalt;
    LINK_CNTL.lc_thread = SAL_THREAD_ERROR;

    return CBX_E_NONE;
}


/**
 * Function:
 *      cbx_link_scan_detach
 * Purpose:
 * Detach a Link scan previously initialized by cbx_link_scan_init()
 * Terminates the link_scan thread, and frees all memory associated
 * with link_scan on the device.
 * All registered handlers are unregistered requiring them
 * to re-register if cbx_link_scan_init is called to reinitialize link_scan.
 *
 * @param link_scan_params  (IN)  Link scan parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously initialized link scan.
 */

int cbx_link_scan_detach ( cbx_link_scan_params_t *link_scan_params )
{
    cbx_ls_cntl_t           *ls_cntl;
    cbx_ls_handler_t        *cb_hdlr;

    LC_CHECK_INIT;

    LOG_INFO(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API : cbx_link_scan_detach()..\n")));

    ls_cntl = &LINK_CNTL;

    CBX_IF_ERROR_RETURN(cbx_link_scan_enable_set(link_scan_params));

    /* Clean up list of handlers */
    while (ls_cntl->lc_handler != NULL) {
        cb_hdlr = ls_cntl->lc_handler;
        ls_cntl->lc_handler = cb_hdlr->lh_next;
        sal_free(cb_hdlr);
    }

    if (ls_cntl->lc_sema != NULL) {
        sal_sem_destroy(ls_cntl->lc_sema);
        ls_cntl->lc_sema = NULL;
    }

    if (ls_cntl->lc_lock != NULL) {
        sal_mutex_destroy(ls_cntl->lc_lock);
        ls_cntl->lc_lock = NULL;
    }
    /* Link scan initialization not complete */
    LINK_CNTL.init = FALSE;
    return CBX_E_NONE;
}


/**
 * Function:
 *      cbx_link_scan_register
 * Purpose:
 * Link scan register
 * This routine is used to register a Link Scan callback routine.
 * Register a handler invoked on a link up or link down transition.
 * If multiple handlers are registered, they are invoked on a
 * link transition in the same order as they were registered.
 *
 * @param link_scan_info  (IN)  Link Scan callback information
 * @param cb              (IN)  Callback function pointer
 *                              see: cbx_link_scan_callback_t
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_register ( cbx_link_scan_info_t      link_scan_info,
                             cbx_link_scan_callback_t  cb )
{
    cbx_ls_cntl_t   *ls_cntl;
    cbx_ls_handler_t *cb_hdlr;

    LC_CHECK_INIT;

    LOG_INFO(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API : cbx_link_scan_register()..\n")));

    ls_cntl = &LINK_CNTL;

    LC_LOCK;

    /* Check if this handler is already registered */
    for (cb_hdlr = ls_cntl->lc_handler; cb_hdlr != NULL;
            cb_hdlr = cb_hdlr->lh_next) {
        if (cb_hdlr->lh_f == cb) {
            LC_UNLOCK;
            return CBX_E_NONE;
        }
    }

    if ((cb_hdlr = sal_alloc(sizeof(cbx_ls_handler_t),
                             "cbx_link_scan_register")) == NULL) {
        LC_UNLOCK;
        return(CBX_E_MEMORY);
    }

    cb_hdlr->lh_next = ls_cntl->lc_handler;
    cb_hdlr->lh_f = cb;
    sal_memcpy(&cb_hdlr->lh_info, &link_scan_info,sizeof(cbx_link_scan_info_t));
    ls_cntl->lc_handler = cb_hdlr;

    LC_UNLOCK;

    return CBX_E_NONE;
}


/**
 * Function:
 *      cbx_link_scan_unregister
 * Purpose:
 * Link scan unregister
 * This routine is used to unregister a Link Scan callback routine.
 *
 * @param cb           (IN)  Link Scan callback routine to unregister
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_unregister ( cbx_link_scan_callback_t  cb )
{
    cbx_ls_cntl_t   *ls_cntl;
    cbx_ls_handler_t *cb_hdlr = NULL, *prevcb_hdlr = NULL;
    int         rv;

    LC_CHECK_INIT;

    LOG_INFO(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API : cbx_link_scan_unregister()..\n")));

    LC_LOCK;

    ls_cntl = &LINK_CNTL;

    for (cb_hdlr = ls_cntl->lc_handler; cb_hdlr != NULL;
            prevcb_hdlr = cb_hdlr, cb_hdlr = cb_hdlr->lh_next) {
        if (cb_hdlr->lh_f == cb) {
            if (prevcb_hdlr == NULL) {
                ls_cntl->lc_handler = cb_hdlr->lh_next;
            } else {
                prevcb_hdlr->lh_next = cb_hdlr->lh_next;
            }
            break;
        }
    }

    if (cb_hdlr != NULL) {
        sal_free(cb_hdlr);
        rv = CBX_E_NONE;
    } else {
        rv = CBX_E_NOT_FOUND;
    }

    LC_UNLOCK;
    return rv;
}


/**
 * Function:
 *      cbx_link_scan_enable_get
 * Purpose:
 * Link scan enable get
 * Retrieve the link scan enable and polling interval.
 *
 * cbx_link_scan_enable_get returns the current scan interval.
 * A returned scan interval of 0 indicates link_scan is disabled.
 *
 * @param link_scan_params  (OUT) Link Scan parameters.
 *                                usec is the minimum time between software
 *                                link scan cycles in micro-seconds.
 *                                0 indicates link_scan is disabled.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_enable_get ( cbx_link_scan_params_t  *link_scan_params )
{
    LC_CHECK_INIT;

    LOG_INFO(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API : cbx_link_scan_enable_get()..\n")));

    link_scan_params->usec = LINK_CNTL.lc_us;

    return CBX_E_NONE;
}


/**
 * Function:
 *      cbx_link_scan_enable_set
 * Purpose:
 * Link scan enable set
 * Enable link scan and set the polling interval.
 *
 * Calling cbx_link_scan_enable_set with a non-zero scanning interval
 * sets the scan interval.
 * If the specified interval is zero, the link_scan task is stopped.
 *
 * @param link_scan_params  (OUT) Link Scan parameters.
 *                                usec is the minimum time between software
 *                                link scan cycles in micro-seconds.
 *                                0 indicates link_scan is disabled.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_enable_set ( cbx_link_scan_params_t  *link_scan_params )
{
    cbx_ls_cntl_t       *ls_cntl;
    int                 rv = CBX_E_NONE;

#ifndef CONFIG_PORT_EXTENDER
    LOG_INFO(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API : cbx_link_scan_enable_set()..\n")));
#endif

    LC_CHECK_INIT;

    ls_cntl = &LINK_CNTL;
    LC_LOCK;

#ifdef CONFIG_PORT_EXTENDER
    if (ls_cntl->lc_thread == SAL_THREAD_ERROR)
#endif
        (void)sal_snprintf(ls_cntl->lc_taskname, sizeof (ls_cntl->lc_taskname),
                 "CBXLINKSCAN");

    if (link_scan_params->usec) {
        if (link_scan_params->usec < CBXI_LINKSCAN_INTERVAL_MIN) {
            link_scan_params->usec = CBXI_LINKSCAN_INTERVAL_MIN;
        }

        ls_cntl->lc_us = link_scan_params->usec;

        if (ls_cntl->lc_thread != SAL_THREAD_ERROR) {
            /* Linkscan is already running;*/
            sal_sem_give(ls_cntl->lc_sema);
        } else if (sal_thread_create(ls_cntl->lc_taskname,
                                     SAL_THREAD_STKSZ,
                                     /* tskIDLE_PRIORITY + */ 1,
                                     cbxi_robo2_link_scan_thread,
                                     NULL) == SAL_THREAD_ERROR) {
            ls_cntl->lc_us = 0;
            rv = CBX_E_MEMORY;
        }
    } else if (ls_cntl->lc_thread != SAL_THREAD_ERROR) {
        /* Disable Linkscan */
        ls_cntl->lc_us = 0;
        sal_sem_give(ls_cntl->lc_sema);
        /* Link scan disable initiated delay for completion */
        sal_sleep(CBXI_LINKSCAN_DISABLE_WAIT);

        /* Disable Linkscan failed */
        while (ls_cntl->lc_thread != SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_FSAL_COMMON,
                      (BSL_META("FSAL API : %s: Thread did not exit\n"),
                       ls_cntl->lc_taskname));
            rv = CBX_E_INTERNAL;
            break;
        }
        (void)sal_snprintf(ls_cntl->lc_taskname, sizeof (ls_cntl->lc_taskname),
                 "NONE");
        ls_cntl->task_status = cbxLinkTaskStatusHalt;
    }
    LC_UNLOCK;

    return(rv);
}


/**
 * Function:
 *      cbxi_link_scan_wakeup
 * Purpose:
 * Wake up the link scan thread for a re-scan.
 * If the link scan thread is not running, this function is a no-op.
 *
 * This function may be called only from interrupt service routines, with
 * interrupts disabled.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbxi_link_scan_wakeup ( void )
{
    cbx_ls_cntl_t *ls_cntl;

    LC_CHECK_INIT;

    ls_cntl = &LINK_CNTL;

    /*
     * Do not use LS_LOCK/LS_UNLOCK here. Locking mutexes is not allowed from
     * ISRs, and will re-enable interrupts as a side effect at best (and cause
     * a deadlock at worst). With interrupts disabled, we can safely assume
     * that neither the link scan thread nor the semaphore will disappear from
     * under us.
     */
    if (ls_cntl->lc_thread != SAL_THREAD_ERROR) {
        sal_sem_give(ls_cntl->lc_sema);
    }

    return CBX_E_NONE;
}


/**
 * Function:
 *      cbx_link_scan_dump
 * Purpose:
 * Link scan dump
 * Display information about link_scan callbacks.
 *
 * @param None
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_dump ( void )
{
    cbx_ls_handler_t *cb_hdlr;
    cbx_ls_cntl_t   *ls_cntl;

    LC_CHECK_INIT;

    LOG_WARN(BSL_LS_FSAL_LINK,
             (BSL_META("FSAL API : cbx_link_scan_dump()..\n")));

    ls_cntl = &LINK_CNTL;

    LOG_WARN(BSL_LS_FSAL_LINK,
             (BSL_META("link_scan task name=%s status=%s\n"),
             ls_cntl->lc_taskname,
             (ls_cntl->task_status==0) ? "Halt" : "Running" ));

    for (cb_hdlr = ls_cntl->lc_handler; cb_hdlr != NULL;
            cb_hdlr = cb_hdlr->lh_next) {
        LOG_WARN(BSL_LS_FSAL_LINK,
                 (BSL_META("FSAL API :Link scan callback function: %s\n"),
                  cb_hdlr->lh_info.name));
    }

    return CBX_E_NONE;
}
