/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *    npa.c
 * Description:
 *    Network Port Adapter Driver for BCM53158
 */

#include <sal_types.h>
#include <sal_alloc.h>
#include <bsl_log/bsl.h>
#include <fsal_int/types.h>
#include <soc/drv.h>
#include <soc/cpu_m7.h>
#include <soc/robo2/common/regacc.h>
#include <sal_console.h>

#ifdef CORTEX_M7

extern void npa_link_chg_handler(int unit);

#endif

/*
 * Function:
 *    extern_intr_enable
 * Description:
 *    Enable external interrupt on given unit for mentioned IRQ
 */
void
extern_intr_enable(int unit, int intr)
{
    uint32 regval = 0;

    if (intr < 32) {
        REG_READ_CRU_CRU_INTERRUPT_ENABLE_REG0_EXTCPUr(unit, &regval);
        regval |= (1 << intr);
        REG_WRITE_CRU_CRU_INTERRUPT_ENABLE_REG0_EXTCPUr(unit, &regval);
    } else {
        intr -= 32;
        REG_READ_CRU_CRU_INTERRUPT_ENABLE_REG1_EXTCPUr(unit, &regval);
        regval |= (1 << intr);
        REG_WRITE_CRU_CRU_INTERRUPT_ENABLE_REG1_EXTCPUr(unit, &regval);

    }
}

/*
 * Function:
 *    extern_intr_check
 * Description:
 *    Check if the IRQn of external interrupt received
 *    Return 0 if received
 */
int
extern_intr_check(int unit, int intr)
{
    uint32 regval = 0;

    if (intr < 32) {
        REG_READ_CRU_CRU_INTERRUPT_STATUS_REG0_EXTCPUr(unit, &regval);
    } else {
        intr -= 32;
        REG_READ_CRU_CRU_INTERRUPT_STATUS_REG1_EXTCPUr(unit, &regval);
    }

    if (regval & (1 << intr)) {
        return 0;
    }

    return -1;
}

/*
 * Function:
 *    extern_intr_ack
 * Description:
 *    Clear the external intrrupt
 */
void
extern_intr_ack(int unit, int intr)
{
    uint32 regval = 0;

    if (intr < 32) {
        REG_READ_CRU_CRU_INTERRUPT_STATUS_REG0_EXTCPUr(unit, &regval);
        regval |= (1 << intr);
        REG_WRITE_CRU_CRU_INTERRUPT_STATUS_REG0_EXTCPUr(unit, &regval);
    } else {
        intr -= 32;
        REG_READ_CRU_CRU_INTERRUPT_STATUS_REG1_EXTCPUr(unit, &regval);
        regval |= (1 << intr);
        REG_WRITE_CRU_CRU_INTERRUPT_STATUS_REG1_EXTCPUr(unit, &regval);

    }
}



/*
 * Function:
 *    gpio_intr_ack
 * Description:
 *    Ack an Intr
 */
void
gpio_intr_ack(int unit, int intr)
{
    REG_WRITE_M7SS_GPIO_GP_INT_CLRr(unit, &intr);
}

#ifdef CORTEX_M7


/*
 * Function:
 *    gpio_intr
 * Description:
 *    Intr handler
 */
void
gpio_handler(void)
{
    uint32 intsts;
    int rv = 0;

    m7_disable_intr();
    do {
        intsts = 0;
        REG_READ_M7SS_GPIO_GP_INT_MSTATr(CBX_AVENGER_PRIMARY, &intsts);
        if (intsts == 0) {
            break;
        } else if (intsts & (1 << 7)) {
            if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                /* Check if IRQn 24 (NPA) on secondary extenal intr */
                rv = extern_intr_check(CBX_AVENGER_SECONDARY, (int)NPA_IRQn);
                if (rv == 0) {
                    /* link change intr handler on secondary */
                    npa_link_chg_handler(CBX_AVENGER_SECONDARY);

                    /* Clear external intr */
                    extern_intr_ack(CBX_AVENGER_SECONDARY, (int)NPA_IRQn);
                }
            }
            gpio_intr_ack(CBX_AVENGER_PRIMARY, intsts);
        } else {
            break;
        }
    } while(1);
    m7_enable_intr();
}

#endif


/*
 * Function:
 *    gpio_intr_enable
 * Description:
 *    Intr enable
 */
void
gpio_intr_enable(int unit, int gpio)
{
    uint32 regval = 0;
    LOG_DEBUG(BSL_LS_SOC_OTHER,
        (BSL_META("GPIO intr en: Unit = %d, GPIO = %d\n"), unit, gpio));

    REG_READ_M7SS_GPIO_GP_INT_TYPEr(unit, &regval);
    regval &= ~(1 << gpio); /*  bit 7 = 0 Edge sensitive for GPIO 7 */
    REG_WRITE_M7SS_GPIO_GP_INT_TYPEr(unit, &regval);

    REG_READ_M7SS_GPIO_GP_INT_DEr(unit, &regval);
    regval &= ~(1 << gpio); /*  bit 7 = 0 Single edge sensitive for GPIO 7 */
    REG_WRITE_M7SS_GPIO_GP_INT_DEr(unit, &regval);

    REG_READ_M7SS_GPIO_GP_INT_EDGEr(unit, &regval);
    regval &= ~(1 << gpio); /*  bit 7 = 0 Falling edge */
    REG_WRITE_M7SS_GPIO_GP_INT_EDGEr(unit, &regval);

    REG_READ_M7SS_GPIO_GP_INT_MSKr(unit, &regval);
    regval |= (1 << gpio); /*  bit 7 = 1 Enable GPIO 7 interrupt  */
    REG_WRITE_M7SS_GPIO_GP_INT_MSKr(unit, &regval);
}
/*
  * Function:
  *    gpio_intr_disable
  * Description:
  *    Intr disable
  */
void
gpio_intr_disable(int unit, int gpio)
{
    uint32 regval = 0;
    LOG_DEBUG(BSL_LS_SOC_OTHER,
        (BSL_META("GPIO intr dis: Unit = %d, GPIO = %d\n"), unit, gpio));
    REG_READ_M7SS_GPIO_GP_INT_MSKr(unit, &regval);
    regval &= ~(1 << gpio); /*  bit 7 = 0 Disable GPIO 7 interrupt  */
    REG_WRITE_M7SS_GPIO_GP_INT_MSKr(unit, &regval);
}

#ifdef CORTEX_M7

/*
  * Function:
  *    gpio_init
  * Description:
  *    GPIO module initr
  */
int
gpio_init(void)
{
    int rv = SOC_E_NONE;

    m7_disable_intr();

    /* Init complete */
    m7_enable_irq(GPIO_IRQn);
    m7_enable_intr();
    LOG_DEBUG(BSL_LS_SOC_OTHER,(BSL_META("GPIO intr init\n")));
    return rv;
}
/*
  * Function:
  *    gpio_uninit
  * Description:
  *    GPIO module uninit
  */
void
gpio_uninit(void)
{
    m7_disable_irq(GPIO_IRQn);
    m7_disable_intr();
    m7_enable_intr();
    LOG_DEBUG(BSL_LS_SOC_OTHER,(BSL_META("GPIO intr uninit\n")));
}
#endif
