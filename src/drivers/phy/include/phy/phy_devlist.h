/*
 * $Copyright:
 * All Rights Reserved.$
 *
 */
#ifdef PHY_DEVLIST_ENTRY
/*
 * PHY_DEVLIST_ENTRY MACROS
 * 
 * Before including this file, define PHY_DEVLIST_ENTRY
 * as a macro to operate on the following parameters:
 *     #define PHY_DEVLIST_ENTRY(_nm,_bd,_fl,_desc,_r0,_r1)
 * 
 *     _nm: Chip Name
 *     _bd: SW Base Driver
 *     _fl: Flags
 *     _desc: Description
 *     _r0: Reserved
 *     _r1: Reserved
 * 
 * Note that this macro will be undefined at the end of this file.
 */

#if PHY_CONFIG_INCLUDE_BCMGPHY == 1 || defined(PHY_DEVLIST_INCLUDE_ALL)
PHY_DEVLIST_ENTRY(BCMGPHY, bcmgphy, PHY_DRIVER_F_INTERNAL, "Internal  GE PHY Driver", 0, 0)
#endif
#if PHY_CONFIG_INCLUDE_BCMI_TSCE_XGXS == 1 || defined(PHY_DEVLIST_INCLUDE_ALL)
PHY_DEVLIST_ENTRY(BCMI_TSCE_XGXS, bcmi_tsce_xgxs, PHY_DRIVER_F_INTERNAL, "Internal TSC Eagle 40G SerDes PHY Driver", 0, 0)
#endif
#if PHY_CONFIG_INCLUDE_BCMI_VIPER_XGXS == 1 || defined(PHY_DEVLIST_INCLUDE_ALL)
PHY_DEVLIST_ENTRY(BCMI_VIPER_XGXS, bcmi_viper_xgxs, PHY_DRIVER_F_INTERNAL, "Internal XGS SerDes PHY Driver", 0, 0)
#endif
/* End PHY_DEVLIST_ENTRY Macros */

#ifdef PHY_DEVLIST_INCLUDE_ALL
#undef PHY_DEVLIST_INCLUDE_ALL
#endif
#undef PHY_DEVLIST_ENTRY
#endif /* PHY_DEVLIST_ENTRY */

