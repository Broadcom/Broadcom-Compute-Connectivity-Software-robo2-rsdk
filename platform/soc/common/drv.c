/*
 * $ID: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * File:
 *     drv.c
 * Purpose:
 *     Robo switch driver
 */

#include <sal_types.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>

/*
 * Driver global variables
 *
 *   soc_control: per-unit driver control structure
 *   soc_ndev: the number of units created
 *   soc_ndev_attached: the number of units attached
 *   soc_family_suffix: a family suffix, configured by soc property soc_family.
 *     Used what parsing soc properties.
 *     If configured, and property.unit, property.chip and property.group was not
 *     found, property.family is searched for.
 */
soc_control_t   *soc_control[SOC_MAX_NUM_DEVICES];
/*soc_persist_t   *soc_persist[SOC_MAX_NUM_DEVICES]; [VKC]*/
int             soc_ndev = 0;
int             soc_ndev_attached = 0;
char            *soc_family_suffix[SOC_MAX_NUM_DEVICES] = {0};


/*
 * @par Function:
 *      soc_init
 * @par Purpose:
 *      Initialize without resetting.
 *
 * @params [in] unit - unit #
 * @return SOC_E_XXX
 */
int
soc_init(int unit)
{
#ifdef BCM_ROBO2_SUPPORT
    return(soc_robo2_do_init(unit, FALSE));
#endif
    return SOC_E_UNAVAIL;
}


/*
 * @par Function:
 *      soc_deinit
 * @par Purpose:
 *      DeInitialize a Device.
 * 
 * @params unit - Device unit #
 * @return SOC_E_XXX
 */
int
soc_deinit(int unit)
{

#ifdef BCM_ROBO2_SUPPORT
     return soc_robo2_deinit(unit);
#endif
    /*inform that detaching device is done*
    SOC_DETACH(unit,0);
    */

    return SOC_E_UNAVAIL;
}

/*
 * @par Function:
 *    soc_reset
 * @par Purpose:
 *    Reset some registers in unit to default working values.
 *
 * @params [in] unit - unit number.
 */
int
soc_reset(int unit)
{
#ifdef BCM_ROBO2_SUPPORT
    return soc_robo2_reset(unit);
#endif
    return SOC_E_UNAVAIL;   
}

/*
 * @par Function:
 *    soc_reset_init
 * @par Purpose:
 *    Reset and Initialize the switch
 *
 * @params [in] unit - unit number.
 */
int
soc_reset_init(int unit)
{
#ifdef BCM_ROBO2_SUPPORT
    return soc_robo2_reset_init(unit);
#endif
    return SOC_E_UNAVAIL;
}

/*
 * @par Function:
 *    soc_attach
 * @par Purpose:
 *    Initialize the soc_control_t structure for a device, allocating all memory
 *    and semaphores required.
 * @params [in] unit - switch unit #.
 * @params [in] detach - Callback function called on detach.
 * @returns  SOC_E_XXX
 */
int
soc_attach(int unit)
{
#ifdef BCM_ROBO2_SUPPORT
    return soc_robo2_attach(unit);
#endif
    return SOC_E_UNAVAIL;
}

/*
 * @par Function:
 *    soc_detach
 * @par Purpose:
 *    Clean up allocated memories and release sems
 *    Free up device access
 * @params [in] unit - switch unit #.
 * @params [in] detach - Callback function called on detach.
 * @returns  SOC_E_XXX
 */
int
soc_detach(int unit)
{
#ifdef BCM_ROBO2_SUPPORT
    return soc_robo2_detach(unit);
#endif
    return SOC_E_UNAVAIL;
}

/*
 * @par Function:
 *    soc_reboot
 * @par Purpose:
 *    Hard reset the Soc
 * @params [in] unit - switch unit #.
 * @returns  SOC_E_XXXX on failure, Does not return on success
 */
int
soc_reboot(int unit)
{
#ifdef BCM_ROBO2_SUPPORT
    return (soc_robo2_reboot(unit));
#endif
    return SOC_E_UNAVAIL;
}



