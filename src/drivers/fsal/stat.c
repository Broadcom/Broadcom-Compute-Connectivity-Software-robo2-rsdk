/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     stat.c
 * * Purpose:
 * *     Robo2 statistics module
 * *
 * * Note:
 * *  Unit number is kept in internal functions for future enhancements.
 * * FSAL APIs which are exposed don't get any unit no and value 0 is passed to
 * * internal functions.
 * *
 * */

#include <bsl_log/bsl.h>
#include <bsl_log/bslenum.h>
#include <soc/drv.h>
#include <fsal/error.h>
#include <fsal/stat.h>
#include <sal_time.h>
#include <sal_types.h>
#include <sal_util.h>
#include <soc/types.h>
#include <fsal_int/stat.h>
#include <fsal_int/types.h>
#include <fsal_int/port.h>

#define COUNTER_FLAGS_DEFAULT   0
#define COUNTER_INTERVAL_DEFAULT 2000000

static int stat_init = FALSE;


/*
 * Function:
 * cbx_stat_init
 *
 * Purpose:
 * This routine is used to initialize the STAT module on the system.
 *
 * @param void
 *
 * @return Error code
 * @retval CBX_E_NONE for success
 */

int cbx_stat_init(void)
{
    pbmp_t          pbmp;
    sal_usecs_t     interval;
    uint32_t        flags;
    int             unit = 0;

    LOG_VERBOSE(BSL_LS_FSAL_STAT,
                (BSL_META_U(unit,
                            "BCM API : bcm_robo_stat_init()..\n")));
    interval = COUNTER_INTERVAL_DEFAULT;
    flags = COUNTER_FLAGS_DEFAULT;

    SOC_PBMP_CLEAR(pbmp);
    SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)){
        SOC_PBMP_OR(pbmp,
                    (PBMP_ALL(CBX_AVENGER_SECONDARY)<< CBX_MAX_PORT_PER_UNIT));
    }

    if(stat_init == FALSE)
    {
        CBX_IF_ERROR_RETURN(soc_robo2_counter_attach(unit));

        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)){
           CBX_IF_ERROR_RETURN(soc_robo2_counter_attach(CBX_AVENGER_SECONDARY));
        }
    }

    CBX_IF_ERROR_RETURN(counter_reset(unit));

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)){
        CBX_IF_ERROR_RETURN(counter_reset(CBX_AVENGER_SECONDARY));
    }

    CBX_IF_ERROR_RETURN(counter_thread_set(unit,
                        COUNTER_THREAD_START, flags, interval, pbmp));
    stat_init = TRUE;

    return CBX_E_NONE;

}
/*
 * Function:
 * cbx_stat_reset
 *
 * Purpose:
 * This routine is used to reset all the STAT entries in the system.
 *
 * @param void
 *
 * @return Error code
 * @retval  CBX_E_NONE for success
 */

int cbx_stat_reset(void)
{
    int unit = 0;
    CBX_IF_ERROR_RETURN(counter_reset(unit));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)){
        CBX_IF_ERROR_RETURN(counter_reset(CBX_AVENGER_SECONDARY));
    }
    return CBX_E_NONE;
}

/*
 * Function:
 * cbx_port_stat_get
 *
 * Purpose:
 * This routine is used to read a 32-bit STAT entries on a port.
 *
 * @param portid      Port Identifier
 *
 * @return Error code
 * @retval CBX_E_NONE for success
 */

int cbx_port_stat_get(cbx_portid_t portid, cbx_port_stat_counter_t type,
                      uint64_t *value)
{
#ifdef CONFIG_EXTERNAL_HOST
    int     sync_hw = TRUE;
#else
    int     sync_hw = FALSE; /* Read software accumulated counters */
#endif
#ifdef BE_HOST
    uint64_t val_lo = 0;
    uint64_t val_hi = 0;
#endif
    int         unit = 0;
    cbx_port_t  port_out;

    LOG_VERBOSE(BSL_LS_FSAL_STAT,
                (BSL_META_U(unit,
                            "BCM API : bcm_robo_stat_get()..\n")));
    CBX_IF_ERROR_RETURN(cbxi_robo2_portid_validate(portid, &port_out));
    CBX_IF_ERROR_RETURN(counter_get
        (unit, port_out, type, sync_hw, value));

#ifdef BE_HOST
    COMPILER_64_ZERO(val_lo);
    COMPILER_64_ZERO(val_hi);
    val_lo = COMPILER_64_LO(*value);
    val_hi = COMPILER_64_HI(*value);
    COMPILER_64_ZERO(*value);
    COMPILER_64_SET(*value, val_lo, val_hi);
#endif

    return CBX_E_NONE;
}

/*
 * Function:
 * cbx_port_stat_mult_get
 *
 * Purpose:
 * This routine is used to read multiple 32-bit STAT entries on a port.
 *
 * @param portid           Port Identifier
 * @param number_of_stats  Number of valid types in the array
 * @param type_array       Type array
 * @param value_array      Value array
 *
 * @return Error code
 * @retval CBX_E_NONE for success
 */

int cbx_port_stat_mult_get(cbx_portid_t    portid, int number_of_stats,
                            cbx_port_stat_counter_t *type_array,
                            uint64_t *value_array)
{
    int stix;

    if (number_of_stats <= 0) {
        return CBX_E_PARAM;
    }

    if ((NULL == type_array) || (NULL == value_array)) {
        return CBX_E_PARAM;
    }

    for (stix = 0; stix < number_of_stats; stix++) {
        CBX_IF_ERROR_RETURN
            (cbx_port_stat_get(portid, type_array[stix],
                                &(value_array[stix])));
    }

    return CBX_E_NONE;


}


/*
 * Function:
 * cbx_port_stat_clear
 *
 * Purpose:
 * This routine is used to clear the STAT entries on a port.
 *
 * @param portid      Port Identifier
 *
 * @return Error code
 * @retval CBX_E_NONE for success
 */

int cbx_port_stat_clear(cbx_portid_t portid)
{
    pbmp_t       pbm;
    uint64_t     val64;
    sal_usecs_t  interval=0;
    uint32_t     flags = 0;
    int          unit = 0;
    cbx_port_t   port_out;

    LOG_VERBOSE(BSL_LS_FSAL_STAT,
                (BSL_META_U(unit,
                            "BCM API : bcm_robo_stat_clear()..\n")));

    CBX_IF_ERROR_RETURN(cbxi_robo2_portid_validate(portid, &port_out));
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port_out);


    /*
     * Force one time synchronization before clear counters in case that
     * the content of sw and hw counter tables not the same.
     */
    CBX_IF_ERROR_RETURN(counter_thread_set
                (unit, COUNTER_THREAD_SYNC, flags,
                interval, pbm));


    COMPILER_64_ZERO(val64);
    CBX_IF_ERROR_RETURN(soc_robo_counter_set_by_port
                (unit, port_out, val64));

    return(CBX_E_NONE);
}



/**
 * STAT vlan clear
 * This routine is used to clear the STAT entries on a vlan.
 *
 * @param vlanid      Vlan Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_stat_clear(cbx_vlanid_t vlanid)
{
    /*Will be implemented later*/
    return(CBX_E_UNAVAIL);
}


/**
 * STAT cosq get
 * This routine is used to read a STAT entries on a cosq.
 *
 * @param cosqid      Cosq Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_stat_get (cbx_cosqid_t             cosqid,
                       cbx_cosq_stat_counter_t  type,
                       uint64_t                *value)
{
    /*Will be implemented later*/
    return(CBX_E_UNAVAIL);
}

/**
 * STAT cosq multi_get
 * This routine is used to read multiple STAT entries on a cosq.
 *
 * @param cosqid           Cosq Identifier
 * @param number_of_stats  Number of valid types in the array
 * @param type_array       Type array
 * @param value_array      Value array
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_stat_mult_get (cbx_cosqid_t             cosqid,
                            int                      number_of_stats,
                            cbx_cosq_stat_counter_t *type_array,
                            uint64_t                *value_array)
{
    /*Will be implemented later*/
    return(CBX_E_UNAVAIL);
}


/**
 * STAT cosq clear
 * This routine is used to clear the STAT entries on a cosq.
 *
 * @param cosqid      Cosq Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_stat_clear (cbx_cosqid_t cosqid)
{
    /*Will be implemented later*/
    return(CBX_E_UNAVAIL);
}


/**
 * STAT meter get
 * This routine is used to read a STAT entries on a meter.
 *
 * @param meterid      Meter Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_meter_stat_get (cbx_meterid_t             meterid,
                        cbx_meter_stat_counter_t  type,
                        uint64_t                 *value)
{
    /*Will be implemented later*/
    return(CBX_E_UNAVAIL);
}

/**
 * STAT meter multi_get
 * This routine is used to read multiple STAT entries on a meter.
 *
 * @param meterid          Meter Identifier
 * @param number_of_stats  Number of valid types in the array
 * @param type_array       Type array
 * @param value_array      Value array
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_meter_stat_mult_get (cbx_meterid_t             meterid,
                             int                       number_of_stats,
                             cbx_meter_stat_counter_t *type_array,
                             uint64_t                 *value_array)
{
    /*Will be implemented later*/
    return(CBX_E_UNAVAIL);
}

/**
 * STAT meter clear
 * This routine is used to clear the STAT entries on a meter.
 *
 * @param meterid     (IN)  Meter Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_meter_stat_clear (cbx_meterid_t meterid)
{
    /*Will be implemented later*/
    return(CBX_E_UNAVAIL);
}


