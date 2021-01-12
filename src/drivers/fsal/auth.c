/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     auth.c
 * * Purpose:
 * *     Robo2 fsal authentication (802.1x) module
 * *
 * */


#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <soc/types.h>
#include <fsal/error.h>
#include <fsal/exception.h>
#include <fsal_int/auth.h>
#include <fsal_int/port.h>
#include <fsal_int/stg.h>
#include <fsal_int/types.h>
#include <fsal_int/slic.h>
#include <fsal_int/port.h>
#include <fsal_int/trap.h>
#include <fsal_int/l2.h>
#include <soc/robo2/common/tables.h>
#include <sal_alloc.h>

extern int cbxi_slicid_map(int unit, cbxi_slic_rule_id_t rule_number,
                cbxi_slicid_t slicid, cbxi_slic_trap_t slic_trap);

static cbx_auth_cntl_t cbx_robo2_auth_cntl;

#define AUTH_CNTL       cbx_robo2_auth_cntl
#define AUTH_INIT                                 \
        if (!cbx_robo2_auth_cntl.init)            \
                    return CBX_E_INIT

/*
 * Function:
 *  cbxi_robo2_auth_port_flags_validate
 * Purpose:
 *  Check if authentication port flags are valid.
 *
 */
int cbxi_robo2_auth_port_flags_validate( uint32_t auth_port_flags )
{
    int       rv = CBX_E_NONE;

    /* Only one type of authentication mode is permitted  */
    if ((auth_port_flags & CBX_AUTH_PORT_UNAUTH ) &&
        (auth_port_flags & CBX_AUTH_PORT_AUTH )) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
              (BSL_META("FSAL API : cbxi_robo2_auth_port_flags_validate()..\
                     invalid authentication mode =0x%x\n"),
                     auth_port_flags));
        return CBX_E_PARAM;
    }

    if (auth_port_flags & CBX_AUTH_PORT_UNAUTH ) {

        /* Only block ingress traffic,ingress-egress traffic modes are
         * permitted
         */
        if ( (auth_port_flags & CBX_AUTH_PORT_UNKNOWN_DROP ) ||
                (auth_port_flags & CBX_AUTH_PORT_UNKNOWN_TRAP ) ||
                (auth_port_flags & CBX_AUTH_PORT_LINK )) {
            rv = CBX_E_PARAM;
        }
    } else if (auth_port_flags & CBX_AUTH_PORT_AUTH ) {

        /* Only unknown drop, unknown trap and link mode are permitted  */
        if ( (auth_port_flags & CBX_AUTH_PORT_BLOCK_IN ) ||
                (auth_port_flags & CBX_AUTH_PORT_BLOCK_INOUT )) {
            rv = CBX_E_PARAM;
        }
    } else {
        rv =  CBX_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *  cbxi_robo2_slic_pgmap_port_udpate
 * Purpose:
 *  Update port in pgmap field of SLICMAP table
 *
 */
STATIC int
cbxi_robo2_slic_pgmap_port_update( int unit,
                                   uint32_t   index,
                                   cbx_port_t port,
                                   uint32_t   flag )
{
    pbmp_t        slicrule_pbmp;

    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, index,
                        &slicrule_pbmp));
    if (flag & CBXI_AUTH_PORT_ADD ) {
        CBX_PBMP_PORT_ADD(slicrule_pbmp, port);
    } else if (flag & CBXI_AUTH_PORT_REMOVE ) {
        CBX_PBMP_PORT_REMOVE(slicrule_pbmp, port);
    } else {
        return CBX_E_PARAM;
    }

    CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, index,
                        slicrule_pbmp));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_slic_eapol_ctrl_pgmap_update
 * Purpose:
 *  Update entire pgmap for all EAPOL control frame entries in SLICMAP table
 *
 */
STATIC int
cbxi_robo2_slic_eapol_ctrl_pgmap_update( int unit,
        uint32_t   flag )
{
    uint32_t      index;
    pbmp_t        slicrule_pbmp;
    cbxi_pgid_t   lpg;
    cbx_portid_t  portid;
    cbx_port_t    port_out;
    pbmp_t        lpg_mask = 0;
    int           tunit;

    /* Convert PBMP_ALL port bitmap to LPG bitmap */
    CBX_PBMP_CLEAR(lpg_mask);
    CBX_PBMP_ITER(PBMP_ALL(unit), portid) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(portid, &port_out,
                                                     &lpg, &tunit));
        CBX_PBMP_PORT_ADD(lpg_mask, lpg);
    }

    /* SLIC indexes to trap EAPOL Control frame entries in SLIC TCAM
     * SLIC_EAPOL_ETYPE        		= 23,  EAPOL etype = 0x888E only
     * SLIC_EAPOL_IEEE         		= 24,  EAPOL IEEE MAC
     * SLIC_EAPOL_UCAST        		= 25,  EAPOL user defined unicast
     * SLIC_EAPOL_MCAST        		= 26,  EAPOL user defined multicast
     */

    for(index = SLIC_EAPOL_IEEE; index<SLIC_EAPOL_ETYPE; index++) {
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_get(unit, index,
                            &slicrule_pbmp));
        if (flag & CBXI_AUTH_SLIC_PGMAP_SET ) {
            /* Add all ports */
            CBX_PBMP_ASSIGN(slicrule_pbmp, lpg_mask);
        } else if (flag & CBXI_AUTH_SLIC_PGMAP_CLEAR ) {
            /* Remove all ports */
            CBX_PBMP_REMOVE(slicrule_pbmp, lpg_mask);
        } else {
            return CBX_E_PARAM;
        }
        CBX_IF_ERROR_RETURN(cbxi_slic_pgmap_set(unit, index,
                            slicrule_pbmp));
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_estget_update
 * Purpose:
 *  Update ESTGET table
 */

STATIC int
cbxi_robo2_estget_update(int unit,
                         cbxi_pgid_t pgid,
                         uint32_t  flag )
{
    uint32 idx;
    uint32 maxidx;
    uint32 status = 0;
    estget_t estget_entry;

    maxidx = soc_robo2_estget_max_index(unit);

    for (idx = 0; idx <= maxidx; idx++) {
        CBX_IF_ERROR_RETURN(soc_robo2_estget_get(unit, idx, &estget_entry));
        if (flag & CBXI_AUTH_PORT_ADD ) {
            CBX_PBMP_PORT_ADD(estget_entry.tx_ena, pgid);
        } else if (flag & CBXI_AUTH_PORT_REMOVE ) {
            CBX_PBMP_PORT_REMOVE(estget_entry.tx_ena, pgid);
        } else {
            return CBX_E_PARAM;
        }

        CBX_IF_ERROR_RETURN(soc_robo2_estget_set(unit, idx, &estget_entry,
                            &status));
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_pglct_udpate
 * Purpose:
 *  Update PGLCT table
 *
 */
STATIC int
cbxi_robo2_pglct_update( int unit,
                         cbxi_pgid_t gpg,
                         uint32_t   disable_learn )
{
    pglct_t       pglct_entry;
    uint32        status  = 0;

    CBX_IF_ERROR_RETURN(soc_robo2_pglct_get(unit, gpg, &pglct_entry));

    if(disable_learn)
    {
        pglct_entry.learn_limit = 0;
    } else {
        pglct_entry.learn_limit = 0x4000;
    }

    CBX_IF_ERROR_RETURN(soc_robo2_pglct_set(unit, gpg,
                                            &pglct_entry, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_trap_udpate
 * Purpose:
 *  Update Trap control table
 *
 */
STATIC int
cbxi_robo2_trap_update( int unit,
                        uint32_t   enable,
                        uint32_t   flags )
{
    cbxi_trap_ctrl_t trap_ctl;

    CBX_IF_ERROR_RETURN(cbxi_trap_control_get(unit, FM_COPY_CPU,
                        &trap_ctl));
    if (enable) {
        if( flags &  CBX_AUTH_PORT_UNKNOWN_TRAP) {
            trap_ctl.trap_flags = CBXI_TRAP_TO_CPU;
        } else {
            trap_ctl.trap_flags = 0;
        }
    } else {
        trap_ctl.trap_flags = 0;
    }
    trap_ctl.drop = enable;
    trap_ctl.term = 0;
    CBX_IF_ERROR_RETURN(cbxi_trap_control_set(unit, MAC_LEARN_LIMIT,
                        &trap_ctl));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_auth_config_update
 * Purpose:
 *  Update AUTH configuration to  hardware
 *
 */
STATIC int
cbxi_robo2_auth_config_update ( int unit,
                                cbx_auth_params_t  *auth_params )
{
    uint32              update_flag = CBXI_AUTH_PORT_REMOVE;
    int                 rv;
    uint32              int_mac[2];
    uint32              clr_eapol_etype = FALSE;
    cbxi_slic_rule_t    slic_rule;
    eth_mac_t           null_mac  = {0x00,0x00,0x00,0x00,0x00,0x00};

    LOG_INFO(BSL_LS_FSAL_AUTH,
             (BSL_META("FSAL API : cbxi_robo2_auth_config_update()..\n")));

    /*
     * Uncontrolled:
     *  eapol control packets are not trapped to CPU, but are bridged and
     *  the default eapol (drop other).Pg_port_map[15:0] is all bits cleared.
     *
     * Unauth:
     * eapol control packets on all ports are trapped to CPU and the default
     * eapol (drop other).Pg_port_map[15:0] is all bits set.
     *
     * For Auth:
     * eapol control packets on all ports are trapped to CPU and the default
     * eapol (drop other).Pg_port_map[15:0] is all bits cleared.
     */

    switch (auth_params->auth_mode) {
    case cbxAuthModeUncontrolled:
        update_flag = CBXI_AUTH_SLIC_PGMAP_CLEAR;
        break;
    case cbxAuthModeUnauth:
    case cbxAuthModeAuth:
        update_flag = CBXI_AUTH_SLIC_PGMAP_SET;
        break;
    default:
        return CBX_E_PARAM;
        break;
    }

    /* Update PGMAP for slic entries SLIC_EAPOL_ETYPE, SLIC_EAPOL_IEEE,
     * SLIC_EAPOL_UCAST and SLIC_EAPOL_MCAST.
     */
    rv = cbxi_robo2_slic_eapol_ctrl_pgmap_update(unit, update_flag);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
              (BSL_META("FSAL API : cbxi_robo2_auth_config_update()..Failed to \
                     update SLICMAP entry \n")));
        return rv;
    }

    /* Update UC_MAC at SLIC_EAPOL_UCAST index */
    if (!CBX_MAC_ADDR_EQUAL(null_mac, auth_params->uc_mac))
    {
        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(
                                    unit, SLIC_EAPOL_UCAST, &slic_rule));

        sal_memset(int_mac, 0, sizeof(int_mac));
        CBX_MAC_ADDR_TO_HI_UINT32(auth_params->uc_mac, int_mac);
        slic_rule.dmac_hi_32   = int_mac[1];
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = int_mac[0];
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_DMAC );

        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_set(
                                    unit, SLIC_EAPOL_UCAST, slic_rule));
        /* Clear EAPOL_ETYPE slic rule */
        clr_eapol_etype = TRUE;
    }

    /* Update MC_MAC at SLIC_EAPOL_MCAST index */
    if (!CBX_MAC_ADDR_EQUAL(null_mac, auth_params->mc_mac))
    {
        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(
                                    unit, SLIC_EAPOL_MCAST, &slic_rule));

        sal_memset(int_mac, 0, sizeof(int_mac));
        CBX_MAC_ADDR_TO_HI_UINT32(auth_params->mc_mac, int_mac);
        slic_rule.dmac_hi_32   = int_mac[1];
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = int_mac[0];
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_DMAC );

        CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_set(
                                    unit, SLIC_EAPOL_MCAST, slic_rule));
        /* Clear EAPOL_ETYPE slic rule */
        clr_eapol_etype = TRUE;
    }

    if (clr_eapol_etype)
    {
        CBX_IF_ERROR_RETURN(cbxi_slicid_map(unit, SLIC_EAPOL_ETYPE,
                           SLICID_USE_DP, SLIC_TRAP_GROUP3));
    }
    else
    {
        CBX_IF_ERROR_RETURN(cbxi_slicid_map(unit, SLIC_EAPOL_ETYPE,
                           SLICID_EAPOL, SLIC_TRAP_GROUP2));
    }
    return CBX_E_NONE;
}

/**************************************************************************
 *                 Authentication FSAL API IMPLEMENTATION                 *
 **************************************************************************/
/**
 * Initialize global authentication parameters.
 *
 * @param auth_params global authentication parameter object
 */
void cbx_auth_params_t_init( cbx_auth_params_t *auth_params )
{
    if ( NULL != auth_params ) {
        sal_memset( auth_params, 0, sizeof ( cbx_auth_params_t ));
    }
    return;
}

/**
 * Initialize authentication port parameters.
 *
 * @param auth_port_mode authentication port parameter object
 */
void cbx_auth_port_mode_t_init(cbx_auth_port_mode_t *auth_port_mode)
{
    if ( NULL != auth_port_mode ) {
        sal_memset( auth_port_mode, 0, sizeof ( cbx_auth_port_mode_t ));
    }
    return;
}

/**
 * Function:
 *      cbx_auth_init
 * Purpose:
 * Initialize Authentication module
 * This routine is used to init auth.
 *
 * @param auth_params   (IN)  Authentication parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_auth_init ( cbx_auth_params_t *auth_params )
{
    uint32              num_units = 1;
    uint32              port = 0;
    int                 unit = CBX_AVENGER_PRIMARY;
    cbxi_auth_port_t    *port_auth;
    int                 rv;

    LOG_INFO(BSL_LS_FSAL_AUTH,
             (BSL_META("FSAL API : cbx_auth_init()..\n")));

    if ( NULL == auth_params ) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
            (BSL_META("FSAL API : cbx_auth_init().. \
                                   Invalid parameter\n")));
        return CBX_E_PARAM;
    }

    sal_memset( &AUTH_CNTL, 0, sizeof ( cbx_auth_cntl_t ));
    sal_memcpy( &AUTH_CNTL.auth_params, auth_params,
                sizeof ( cbx_auth_params_t ));

    /* Check for cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for ( unit = 0; unit < num_units; unit++ ) {
        for ( port = 0; port < CBX_MAX_PORT_PER_UNIT; port++ ) {
            port_auth = &AUTH_CNTL.port_auth[unit][port];
            sal_memset( port_auth, 0, sizeof ( cbxi_auth_port_t ));
            port_auth->in_use = FALSE;
        }

        rv = cbxi_robo2_auth_config_update(unit, auth_params);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_AUTH,
                      (BSL_META("FSAL API : cbx_auth_init()..Failed to \
                         update auth configuration to hardware \n")));
            return rv;
        }
        CBX_PBMP_CLEAR(AUTH_CNTL.auth_pbmp[unit]);
        AUTH_CNTL.trap2cp_enable[unit] = FALSE;
    }

    /* Mark auth init complete */
    AUTH_CNTL.init = TRUE;

    return CBX_E_NONE;
}

/**
 * Function:
 *      cbx_auth_detach
 * Purpose:
 * Detach a Authentication previously initialized by cbx_auth_init()
 *
 * @param auth_params   (IN)  Authentication parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously initialized Authentication module
 */

int cbx_auth_detach ( cbx_auth_params_t *auth_params )
{
    uint32              num_units = 1;
    uint32              port = 0;
    int                 unit = CBX_AVENGER_PRIMARY;
    cbxi_auth_port_t    *port_auth;
    cbx_auth_mode_t     auth_mode;
    int                 rv;

    AUTH_INIT;

    LOG_INFO(BSL_LS_FSAL_AUTH,
             (BSL_META("FSAL API : cbx_auth_detach()..\n")));

    if ( NULL == auth_params ) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
            (BSL_META("FSAL API : cbx_auth_detach().. \
                                   Invalid parameter\n")));
        return CBX_E_PARAM;
    }

    /* Check for cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for ( unit = 0; unit < num_units; unit++ ) {
        for ( port = 0; port < CBX_MAX_PORT_PER_UNIT; port++ ) {
            port_auth = &AUTH_CNTL.port_auth[unit][port];
            sal_memset( port_auth, 0, sizeof ( cbxi_auth_port_t ));
            port_auth->in_use = FALSE;
        }

        /* Set AUTH mode as Uncontrolled, irrespective of user set
         * mode to clear configuration done for AUTH module
         */
        auth_mode  = auth_params->auth_mode;
        auth_params->auth_mode  = cbxAuthModeUncontrolled;

        rv = cbxi_robo2_auth_config_update(unit, auth_params);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_AUTH,
                      (BSL_META("FSAL API : cbx_auth_init()..Failed to \
                         update auth configuration to hardware \n")));
            return rv;
        }

        auth_params->auth_mode = auth_mode;
        CBX_PBMP_CLEAR(AUTH_CNTL.auth_pbmp[unit]);
    }

    /* Mark auth init not done */
    AUTH_CNTL.init = FALSE;

    return CBX_E_NONE;
}


/**
 * Function:
 *      cbx_auth_mode_get
 * Purpose:
 * Authentication mode get
 * This routine is used to get the Authentication configuration information.
 *
 * @param auth_params   (OUT)  Authentication parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_auth_mode_get ( cbx_auth_params_t *auth_params )
{
    AUTH_INIT;

    LOG_INFO(BSL_LS_FSAL_AUTH,
             (BSL_META("FSAL API : cbx_auth_mode_get()..\n")));

    if ( NULL == auth_params ) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
            (BSL_META("FSAL API : cbx_auth_mode_get().. \
                                   Invalid parameter\n")));
        return CBX_E_PARAM;
    }

    sal_memcpy( auth_params, &AUTH_CNTL.auth_params,
                sizeof ( cbx_auth_params_t ));
    return CBX_E_NONE;
}


/**
 * Function:
 *      cbx_auth_mode_set
 * Purpose:
 * Authentication mode set
 * This routine is used to set the Authentication configuration information.
 *
 * @param auth_params   (IN)   Authentication parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_auth_mode_set ( cbx_auth_params_t *auth_params )
{
    uint32              num_units = 1;
    int                 unit = CBX_AVENGER_PRIMARY;
    int                 rv;

    AUTH_INIT;

    LOG_INFO(BSL_LS_FSAL_AUTH,
             (BSL_META("FSAL API : cbx_auth_mode_set()..\n")));

    if ( NULL == auth_params ) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
            (BSL_META("FSAL API : cbx_auth_mode_set().. \
                                   Invalid parameter\n")));
        return CBX_E_PARAM;
    }

    sal_memcpy( &AUTH_CNTL.auth_params, auth_params,
                sizeof ( cbx_auth_params_t ));

    /* Check for cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for ( unit = 0; unit < num_units; unit++ ) {
        rv = cbxi_robo2_auth_config_update(unit, auth_params);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_AUTH,
                      (BSL_META("FSAL API : cbx_auth_mode_set()..Failed to \
                         update auth configuration to hardware \n")));
            return rv;
        }
    }
    return CBX_E_NONE;
}


/**
 * Function:
 *      cbx_auth_port_get
 * Purpose:
 * Authentication Port get
 * This routine is used get the Authentication mode of a port.
 *
 * @param portid     (IN)  Port Identifier
 * @param port_mode  (OUT) Port Authentication mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_auth_port_get ( cbx_portid_t          portid,
                        cbx_auth_port_mode_t *port_mode )
{
    cbx_port_t port_out;
    cbxi_pgid_t lpg;
    int unit = CBX_AVENGER_PRIMARY;
    int rv;

    AUTH_INIT;

    LOG_INFO(BSL_LS_FSAL_AUTH,
             (BSL_META("FSAL API : cbx_auth_port_get()..\n")));

    if ( NULL == port_mode ) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
            (BSL_META("FSAL API : cbx_auth_port_get().. \
                                   Invalid parameter\n")));
        return CBX_E_PARAM;
    }

    /* LAG port is not permitted */
    if (CBX_PORTID_IS_TRUNK(portid)) {
        return CBX_E_PORT;
    }

    rv = cbxi_robo2_port_validate(portid, &port_out, &lpg, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
                  (BSL_META("FSAL API : cbx_auth_port_get()... \
                                      Port validation failed \n")));
        return CBX_E_PARAM;
    }

    /* CPU port is not permitted */
    if (IS_CPU_PORT(unit,port_out) ) {
        return CBX_E_PORT;
    }

    port_mode->flags = AUTH_CNTL.port_auth[unit][port_out].flags;

    LOG_VERBOSE(BSL_LS_FSAL_AUTH,
                (BSL_META("FSAL API : cbx_auth_port_get()...\
                          port=%d, mode=0x%x\n"),
                 port_out, port_mode->flags));

    return CBX_E_NONE;
}


/**
 * Function:
 *      cbx_auth_port_set
 * Purpose:
 * Authentication Port set
 * This routine is used set the Authentication mode of a port.
 *
 * @param portid     (IN)  Port Identifier
 * @param port_mode  (IN)  Port Authentication mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_auth_port_set ( cbx_portid_t          portid,
                        cbx_auth_port_mode_t *port_mode )
{
    cbx_port_t  port_out;
    cbxi_pgid_t lpg;
    cbxi_pgid_t gpg;
    uint32_t    old_flags;
    int         unit = CBX_AVENGER_PRIMARY;
    int         num_units = 1;
    int         tunit = 0;
    int         rv;

    AUTH_INIT;

    LOG_INFO(BSL_LS_FSAL_AUTH,
             (BSL_META("FSAL API : cbx_auth_port_set()..\n")));

    if ( NULL == port_mode ) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
            (BSL_META("FSAL API : cbx_auth_port_set().. \
                                   Invalid parameter\n")));
        return CBX_E_PARAM;
    }

    /* LAG port is not permitted */
    if (CBX_PORTID_IS_TRUNK(portid)) {
        return CBX_E_PORT;
    }

    rv = cbxi_robo2_port_validate(portid, &port_out, &lpg, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
                  (BSL_META("FSAL API : cbx_auth_port_set()... \
                                      Port validation failed \n")));
        return CBX_E_PORT;
    }

    rv = cbxi_robo2_lpg_to_gpg(unit, lpg, &gpg);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
                  (BSL_META("FSAL API : cbx_auth_port_set()... \
                                      lpg2gpg get failed \n")));
        return CBX_E_PORT;
    }

    /* CPU port is not permitted */
    if (IS_CPU_PORT(unit,port_out) ) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
                  (BSL_META("FSAL API : cbx_auth_port_set()... \
                                      CPU Port not permitted  \n")));
        return CBX_E_PORT;
    }

    /* Validate authentication port flags */
    rv = cbxi_robo2_auth_port_flags_validate(port_mode->flags);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_AUTH,
                  (BSL_META("FSAL API : cbx_auth_port_set..authentication port \
                              flags validate failed\n")));
        return rv;
    }

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    /* Clear all old settings for old AUTH mode before
     * applying settings for new AUTh mode
     */
    if (AUTH_CNTL.port_auth[unit][port_out].in_use == TRUE) {
        old_flags = AUTH_CNTL.port_auth[unit][port_out].flags;
        AUTH_CNTL.port_auth[unit][port_out].flags = port_mode->flags;

        if ( old_flags & CBX_AUTH_PORT_UNAUTH ) {

            /* Configure hardware to clear block ingress traffic on
             * unauthorized port
             */
            if ( (old_flags & CBX_AUTH_PORT_BLOCK_IN ) ||
                    (old_flags & CBX_AUTH_PORT_BLOCK_INOUT )) {

                /* Clear pgmap in SLICMAP */
                rv = cbxi_robo2_slic_pgmap_port_update(unit,
                                                       SLIC_EAPOL_DROP,
                                             lpg, CBXI_AUTH_PORT_REMOVE);
                if (CBX_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_FSAL_AUTH,
                         (BSL_META("FSAL API : cbx_auth_port_set()..Failed to \
                                 update SLICMAP entry \n")));
                    return rv;
                }
            }

            /* Clear hardware config to not block both ingress
             * and egress traffic on unauthorized port
             */
            if ( old_flags & CBX_AUTH_PORT_BLOCK_INOUT ) {

                /* Add port to pgmap in estget */
                rv = cbxi_robo2_estget_update(unit, lpg,
                                              CBXI_AUTH_PORT_ADD);
                if (CBX_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_FSAL_AUTH,
                          (BSL_META("FSAL API : cbx_auth_port_set()..Failed to \
                                 update ESTGET entry \n")));
                    return rv;
                }
            }
        } else if ( old_flags & CBX_AUTH_PORT_AUTH ) {

            if ( (old_flags & CBX_AUTH_PORT_UNKNOWN_DROP) ||
                    (old_flags & CBX_AUTH_PORT_UNKNOWN_TRAP)) {

                /*  Disable "disable_learn" field in PGLCT */
                for(tunit = 0;tunit<num_units;tunit++) {
                    rv = cbxi_robo2_pglct_update(tunit, gpg, FALSE );
                    if (CBX_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_FSAL_AUTH,
                                 (BSL_META("FSAL API : cbx_auth_port_set()..\
                                            Failed to update PGLCT entry \n")));
                        return rv;
                    }
                }

                CBX_PBMP_PORT_REMOVE(AUTH_CNTL.auth_pbmp[unit],port_out);

                if ((AUTH_CNTL.trap2cp_enable[unit] == TRUE) &&
                (CBX_PBMP_IS_NULL(AUTH_CNTL.auth_pbmp[CBX_AVENGER_PRIMARY])) &&
                (CBX_PBMP_IS_NULL(AUTH_CNTL.auth_pbmp[CBX_AVENGER_SECONDARY]))){
                    for(tunit = 0;tunit<num_units;tunit++) {
                       /* Clear trap control config to trap/drop unknown
                        * traffic on authorized port
                        */
                        rv = cbxi_robo2_trap_update( tunit, FALSE, old_flags);
                        if (CBX_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_FSAL_AUTH,
                                     (BSL_META("FSAL API : cbx_auth_port_set()\
                                            ..Failed to update TCT entry \n")));
                            return rv;
                        }
                        AUTH_CNTL.trap2cp_enable[tunit] = FALSE;
                    }
                }
            }
        }
    } else {
        AUTH_CNTL.port_auth[unit][port_out].in_use = TRUE;
        AUTH_CNTL.port_auth[unit][port_out].flags = port_mode->flags;
    }

    if ( port_mode->flags & CBX_AUTH_PORT_UNAUTH ) {

        /* Configure hardware to block ingress traffic on
         * unauthorized port
         */
        if ( (port_mode->flags & CBX_AUTH_PORT_BLOCK_IN ) ||
                (port_mode->flags & CBX_AUTH_PORT_BLOCK_INOUT )) {

            /* Add port to pgmap in SLICMAP */
            rv = cbxi_robo2_slic_pgmap_port_update(unit, SLIC_EAPOL_DROP,
                                              lpg, CBXI_AUTH_PORT_ADD);
            if (CBX_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_FSAL_AUTH,
                          (BSL_META("FSAL API : cbx_auth_port_set()..Failed to \
                             update SLICMAP entry \n")));
                return rv;
            }
        }
        /* Configure hardware to block egress
         *  traffic on unauthorized port
         */
        if ( port_mode->flags & CBX_AUTH_PORT_BLOCK_INOUT ) {

            /* Remove port from pgmap in estget */
            rv = cbxi_robo2_estget_update(unit, lpg,
                                          CBXI_AUTH_PORT_REMOVE);
            if (CBX_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_FSAL_AUTH,
                          (BSL_META("FSAL API : cbx_auth_port_set()..Failed to \
                             update ESTGET entry \n")));
                return rv;
            }
        }
    } else if ( port_mode->flags & CBX_AUTH_PORT_AUTH ) {

        /* Remove port from pgmap in SLICMAP */
        rv = cbxi_robo2_slic_pgmap_port_update(unit, SLIC_EAPOL_DROP,
                                               lpg, CBXI_AUTH_PORT_REMOVE);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_AUTH,
                      (BSL_META("FSAL API : cbx_auth_port_set()..Failed to \
                         update SLICMAP entry \n")));
            return rv;
        }

        if ( (port_mode->flags & CBX_AUTH_PORT_UNKNOWN_DROP) ||
                (port_mode->flags & CBX_AUTH_PORT_UNKNOWN_TRAP)) {

            /* Enable disable_learn field in PGLCT */
            for(tunit = 0;tunit<num_units;tunit++) {
                /* Enable disable_learn field in PGLCT */
                rv = cbxi_robo2_pglct_update(tunit, gpg, TRUE );
                if (CBX_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_FSAL_AUTH,
                              (BSL_META("FSAL API : cbx_auth_port_set()..Failed\
                                 to update PGLCT entry \n")));
                    return rv;
                }
            }

            CBX_PBMP_PORT_ADD(AUTH_CNTL.auth_pbmp[unit],port_out);

            for(tunit = 0;tunit<num_units;tunit++) {
                if (AUTH_CNTL.trap2cp_enable[tunit] == FALSE) {
                    rv = cbxi_robo2_trap_update( tunit, TRUE, port_mode->flags);

                    if (CBX_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_FSAL_AUTH,
                          (BSL_META("FSAL API : cbx_auth_port_set()..Failed to \
                                     update TCT entry \n")));
                        return rv;
                    }
                    AUTH_CNTL.trap2cp_enable[tunit] = TRUE;
                }
            }
        }
    } else {
        return CBX_E_PARAM;
    }

    return CBX_E_NONE;
}
