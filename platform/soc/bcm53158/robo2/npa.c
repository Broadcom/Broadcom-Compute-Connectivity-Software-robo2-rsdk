/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * File:
 *    npa.c
 * Description:
 *    Network Port Adapter Driver for BCM53158
 */

#include <sal_types.h>
#include <sal_alloc.h>
#include <shared/pbmp.h>
#include <bsl_log/bsl.h>
#include <fsal_int/types.h>
#include <soc/drv.h>
#include <soc/cpu_m7.h>
#include <soc/robo2/common/regacc.h>
#include <fsal/link.h>
#include <fsal_int/link.h>
#include <sal_console.h>

#ifdef CORTEX_M7

/*
 * Function:
 *    npa_intr_ack
 * Description:
 *    Ack an Intr
 */
void
npa_intr_ack(int unit, int intr)
{
    REG_WRITE_NPA_NPA_LNK_CHG_INT_STSr(unit, &intr);
}

/*
 * Function:
 *    npa_link_chg_handler
 * Description:
 *    link change handler
 */
void
npa_link_chg_handler(int unit)
{
    uint32 intsts;
    uint32 linksts;

    do {
        intsts = 0;
        REG_READ_NPA_NPA_LNK_CHG_INT_STSr(unit, &intsts);
        if (intsts == 0) {
            break;
        } else {
            REG_READ_NPA_NPA_RX_LINK_STATUSr(unit, &linksts);
            /* Schedule link scan immediately */
            cbxi_link_scan_wakeup();
            npa_intr_ack(unit, intsts);
        }
    } while(1);
}



/*
 * Function:
 *    npa_intr
 * Description:
 *    Intr handler
 */
void
npa_handler(void)
{
    m7_disable_intr();
    npa_link_chg_handler(CBX_AVENGER_PRIMARY);
    m7_enable_intr();
}

/*
 * Function:
 *    npa_intr_enable
 * Description:
 *    Intr enable
 */
void
npa_intr_enable(int unit, int port)
{
    uint32 regval = 0;

    LOG_DEBUG(BSL_LS_SOC_OTHER,
        (BSL_META("NPA intr en: Unit = %d, port = %d\n"), unit, port));
    REG_READ_NPA_NPA_LNK_CHG_INT_ENr(unit, &regval);
    regval |= (1 << port);
    REG_WRITE_NPA_NPA_LNK_CHG_INT_ENr(unit, &regval);
}
/*
  * Function:
  *    npa_intr_disable
  * Description:
  *    Intr disable
  */
void
npa_intr_disable(int unit, int port)
{
    uint32 regval = 0;

    LOG_DEBUG(BSL_LS_SOC_OTHER,
        (BSL_META("NPA intr dis: Unit = %d, port = %d\n"), unit, port));

    REG_READ_NPA_NPA_LNK_CHG_INT_ENr(unit, &regval);
    regval &= ~(1 << port);
    REG_WRITE_NPA_NPA_LNK_CHG_INT_ENr(unit, &regval);
}
/*
  * Function:
  *    npa_init
  * Description:
  *    NPA module initr
  */
int
npa_init(void)
{
    int rv = SOC_E_NONE;

    m7_disable_intr();

    /* Init complete */
    m7_enable_irq(NPA_IRQn);
    m7_enable_intr();
    LOG_DEBUG(BSL_LS_SOC_OTHER,(BSL_META("NPA intr init\n")));
    return rv;
}
/*
  * Function:
  *    npa_uninit
  * Description:
  *    NPA module uninit
  */
void
npa_uninit(void)
{
    uint32 regval = 0;
    int unit = 0;

    m7_disable_irq(NPA_IRQn);
    m7_disable_intr();
    REG_WRITE_NPA_NPA_LNK_CHG_INT_ENr(unit, &regval);
    m7_enable_intr();
    LOG_DEBUG(BSL_LS_SOC_OTHER,(BSL_META("NPA intr uninit\n")));
}

#endif
