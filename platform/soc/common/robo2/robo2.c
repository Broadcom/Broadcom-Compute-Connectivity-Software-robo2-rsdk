/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:        robo2.c
 * Purpose:
 * Requires:
 */
#include "bsl.h"

#include <soc/drv.h>
#include <soc/robo2.h>
#include <soc/error.h>
#include <soc/debug.h>


#ifdef BCM_ROBO2_SUPPORT

/*
 * Robo chip driver functions.  Common across Robo devices for now.
 * These may get broken out by chip in the future, but not needed yet.
 */
soc_functions_t soc_robo2_drv_funs = {
    soc_robo2_misc_init,
    soc_robo2_mmu_init,
    soc_robo2_age_timer_get,
    soc_robo2_age_timer_max_get,
    soc_robo2_age_timer_set
};


/* soc_misc_init() :
 *  - allowed user to do the init by chip dependant configuration.
 *
 *  Note :
 *   1. below routine is for all Roob chip related init routine.
 *   2. different robo2 chip init section may separated by
 *      "SOC_IS_ROBO53xx(unit)"
 */
int
soc_robo2_misc_init(int unit)
{
    int             rv = SOC_E_NONE;
#if 0
    uint32          temp;
    uint32          reg_value = 0;
    soc_pbmp_t pbmp;
    if (SOC_IS_STARFIGHTER3(unit)) {

        /* Configure WAN ports if any */
        pbmp = soc_property_get_pbmp(unit, spn_PBMP_WAN_PORT, 0);
        SOC_PBMP_AND(pbmp, PBMP_ALL(unit));
        if (SOC_PBMP_NOT_NULL(pbmp)) {
            rv = REG_READ_WAN_PORT_SELr(unit, &reg_value);
            SOC_IF_ERROR_RETURN(rv);
            temp = SOC_PBMP_WORD_GET(pbmp, 0);
            soc_WAN_PORT_SELr_field_set(unit, &reg_value, WAN_SELECTf, &temp);
            temp = 1;
            soc_WAN_PORT_SELr_field_set(unit, &reg_value, EN_MAN2WANf, &temp);
            rv = REG_WRITE_WAN_PORT_SELr(unit, &reg_value);
            SOC_IF_ERROR_RETURN(rv);
        }

        /* Enable dual-imp mode */
        temp = soc_property_get(unit, spn_DUAL_IMP_ENABLE, 0);

        if (SOC_PBMP_MEMBER(pbmp, 5)) {
            /* Port 5 can be selected WAN port only when dual-imp disabled */
            temp = 0;
        }

        if (temp) {
            /* Dual-IMP */
            temp = 0x3;
        } else {
            /* Single-IMP */
            temp = 0x2;
        }

        rv = REG_READ_GMNGCFGr(unit, &reg_value);
        SOC_IF_ERROR_RETURN(rv);
        soc_GMNGCFGr_field_set(unit, &reg_value, FRM_MNGPf, &temp);

        rv = REG_WRITE_GMNGCFGr(unit, &reg_value);
        SOC_IF_ERROR_RETURN(rv);
    }


    /* TB misc_init :
     *  1. Port Mask table reset.
     *  2. Flow Control init :
     *      - global XOFF enable.
     *      - Port basis XOFF disable.
     *  3. User MAC address
     */
    if (SOC_IS_TBX(unit)) {
        uint64  reg64_val;
        soc_pbmp_t  mport_vctr;
        uint8       mport_addr[6];

        /* Reset PORTMASK */
        SOC_IF_ERROR_RETURN(DRV_MEM_CLEAR(unit, DRV_MEM_PORTMASK));

        /* Flow control init : enable global XOFF */
        temp = 1;
        SOC_IF_ERROR_RETURN(REG_READ_NEW_CONTROLr(unit, &reg_value));
        SOC_IF_ERROR_RETURN(soc_NEW_CONTROLr_field_set(
                                unit, &reg_value, EN_SW_FLOW_CONf, &temp));
        SOC_IF_ERROR_RETURN(REG_WRITE_NEW_CONTROLr(unit, &reg_value));

        /* Flow control init : diable XOFF on each port */
        COMPILER_64_ZERO(reg64_val);
        SOC_IF_ERROR_RETURN(REG_WRITE_SW_XOFF_PORT_CTLr(unit, &reg64_val));

        /* User MAC addresses init process
         *  1. clear user address(global will be disabled if all user
         *      addresses were deleted)
         *  2. Set the the VLAN bypass default setting about user address.
         */
        SOC_PBMP_CLEAR(mport_vctr);
        ENET_SET_MACADDR(mport_addr, _soc_mac_all_zeroes);
        SOC_IF_ERROR_RETURN(DRV_MAC_SET(
                                unit, mport_vctr, DRV_MAC_MULTIPORT_0, mport_addr, 0));
        SOC_IF_ERROR_RETURN(DRV_MAC_SET(
                                unit, mport_vctr, DRV_MAC_MULTIPORT_1, mport_addr, 0));

        /* default at VLAN bypass l2 address is for the lagcy support
         *  - DVAPI for all robo2 chip will test such bypass behavior
         */
        SOC_IF_ERROR_RETURN(DRV_VLAN_PROP_SET(
                                unit, DRV_VLAN_PROP_BYPASS_L2_USER_ADDR, TRUE));
    }

    /* reset the Traffic remarking on Non-ResEPacket */
    if (SOC_IS_ROBO_ARCH_VULCAN(unit)) {
        soc_port_t  port;
        uint32      pri;

        /* bcm5395 on this feature is not implemented, so this reset exclude
         * bcm5395 related setting.
         */

        /* reset the control register */
        if (SOC_IS_NORTHSTARPLUS(unit) || SOC_IS_STARFIGHTER3(unit)) {
            rv = REG_READ_TRREG_CTRL0r(unit, &reg_value);
        } else {
            rv = REG_READ_TRREG_CTRLr(unit, &reg_value);
        }
        SOC_IF_ERROR_RETURN(rv);
        temp = 0;
        if (SOC_IS_NORTHSTARPLUS(unit) || SOC_IS_STARFIGHTER3(unit)) {
            soc_TRREG_CTRL0r_field_set(unit, &reg_value, PCP_RMK_ENf, &temp);
        } else {
            soc_TRREG_CTRLr_field_set(unit, &reg_value, PCP_RMK_ENf, &temp);
        }
        if (!(SOC_IS_LOTUS(unit) || SOC_IS_BLACKBIRD2(unit))) {
            if (SOC_IS_NORTHSTARPLUS(unit) || SOC_IS_STARFIGHTER3(unit)) {
                soc_TRREG_CTRL0r_field_set(unit, &reg_value,
                                           CFI_RMK_ENf, &temp);
            } else {
                soc_TRREG_CTRLr_field_set(unit, &reg_value, CFI_RMK_ENf, &temp);
            }
        }

        if (SOC_IS_NORTHSTARPLUS(unit) || SOC_IS_STARFIGHTER3(unit)) {
            rv = REG_WRITE_TRREG_CTRL0r(unit, &reg_value);
        } else {
            rv = REG_WRITE_TRREG_CTRLr(unit, &reg_value);
        }
        SOC_IF_ERROR_RETURN(rv);

        /* reset the TC2PCP mapping */
        PBMP_ALL_ITER(unit, port) {
            for (pri = 0; pri <= 7; pri++) {

                /* the new-pri is formed as {CFI(bit4),PRI(bit3-bit0)}
                 *  - in the reset value, the CFI is rewrite to 0 always!
                 *      (default is 1 on the RV=1 field)
                 */
                SOC_IF_ERROR_RETURN(DRV_PORT_PRI_MAPOP_SET(unit, port,
                                    DRV_PORT_OP_NORMAL_TC2PCP, pri, 0, pri, 0));

                /* outband TC2PCP is supported on bcm53115 only */
                if (SOC_IS_VULCAN(unit) || SOC_IS_STARFIGHTER(unit) ||
                        SOC_IS_POLAR(unit) || SOC_IS_NORTHSTAR(unit) ||
                        SOC_IS_NORTHSTARPLUS(unit) || SOC_IS_STARFIGHTER3(unit)) {
                    SOC_IF_ERROR_RETURN(DRV_PORT_PRI_MAPOP_SET(unit, port,
                                        DRV_PORT_OP_OUTBAND_TC2PCP, pri, 0, pri, 0));

                }
            }
        }
    }

    /* Enable the SA learning of reserved mutilcasts */
    if (SOC_IS_ROBO_ARCH_VULCAN(unit) || SOC_IS_DINO(unit)) {

        temp = 1;
        SOC_IF_ERROR_RETURN(
            REG_READ_RSV_MCAST_CTRLr(unit, &reg_value));
        soc_RSV_MCAST_CTRLr_field_set(unit, &reg_value,
                                      EN_RES_MUL_LEARNf, &temp);
        SOC_IF_ERROR_RETURN(
            REG_WRITE_RSV_MCAST_CTRLr(unit, &reg_value));
    }
#endif

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_robo2_misc_init: OK\n")));

    return rv;
}

int
soc_robo2_mmu_init(int unit)
{
    return SOC_E_NONE;
}

int
soc_robo2_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    return SOC_E_NONE;
}

int
soc_robo2_age_timer_max_get(int unit, int *max_seconds)
{
    return SOC_E_NONE;
}

int
soc_robo2_age_timer_set(int unit, int age_seconds, int enable)
{
    return SOC_E_NONE;
}

#if 0
/*
 * soc_robo2_64_val_to_pbmp() :
 *     -- Get port bitmap from unsigned 64-bits integer variable.
 *
 * unit: unit
 * *pbmp: (OUT) returned port bitmap
 * value64: (IN) data value for transfering into port bitmap
 */
int
soc_robo2_64_val_to_pbmp(int unit, soc_pbmp_t *pbmp, uint64 value64)
{
    uint32 value32;

    COMPILER_64_TO_32_LO(value32, value64);
    SOC_PBMP_WORD_SET(*pbmp, 0, value32);
    if (SOC_INFO(unit).port_num > 32) {
        COMPILER_64_TO_32_HI(value32, value64);
        SOC_PBMP_WORD_SET(*pbmp, 1, value32);
    } else {
        SOC_PBMP_WORD_SET(*pbmp, 1, 0);
    }

    return SOC_E_NONE;
}

/*
 * soc_robo2_64_pbmp_to_val() :
 *     -- Transfer port bitmap into unsigned 64-bits integer variable.
 *
 * unit: unit
 * *pbmp: (IN) returned port bitmap
 * *value64: (OUT) data value for transfering into port bitmap
 */
int
soc_robo2_64_pbmp_to_val(int unit, soc_pbmp_t *pbmp, uint64 *value64)
{
    uint32 value_h, value_l;

    value_l = SOC_PBMP_WORD_GET(*pbmp, 0);
    if (SOC_INFO(unit).port_num > 32) {
        value_h = SOC_PBMP_WORD_GET(*pbmp, 1);
    } else {
        value_h = 0;
    }
    COMPILER_64_SET(*value64, value_h, value_l);

    return SOC_E_NONE;
}

#endif

int
soc_robo2_loop_detect_enable_set(int unit, int enable)
{
#if 0
    uint32 reg_value = 0;
    /* enable Loop Detection */
    SOC_IF_ERROR_RETURN(REG_READ_LPDET_CFGr(unit, &reg_value));

    /* enable/disable loop detection */
    SOC_IF_ERROR_RETURN(soc_LPDET_CFGr_field_set
                        (unit, &reg_value, EN_LPDETf, (uint32 *)&enable));

    SOC_IF_ERROR_RETURN(REG_WRITE_LPDET_CFGr(unit, &reg_value));
#endif

    return SOC_E_NONE;
}

int
soc_robo2_loop_detect_enable_get(int unit, int *enable)
{
#if 0
    uint32 reg_value = 0;

    SOC_IF_ERROR_RETURN(REG_READ_LPDET_CFGr(unit, &reg_value));

    SOC_IF_ERROR_RETURN(soc_LPDET_CFGr_field_get
                        (unit, &reg_value, EN_LPDETf, (void *)&enable));
#endif

    return SOC_E_NONE;
}


int
soc_robo2_loop_detect_pbmp_get(int unit, soc_pbmp_t *pbmp)
{

#if 0
    uint32 reg_value32, pbmp_value32;
    SOC_IF_ERROR_RETURN(REG_READ_LED_PORTMAPr(unit, &reg_value32));

    SOC_IF_ERROR_RETURN(soc_LED_PORTMAPr_field_get
                        (unit, &reg_value32, LED_WARNING_PORTMAPf, &pbmp_value32));

    SOC_PBMP_WORD_SET(*pbmp, 0, pbmp_value32);
#endif

    return SOC_E_NONE;
}

#endif  /* BCM_ROBO2_SUPPORT */
