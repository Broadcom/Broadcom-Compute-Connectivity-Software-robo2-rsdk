/*
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 *
 * $Copyright:
 * All Rights Reserved.$
 *
 */

#ifndef __PHY_CONFIG_CHIPS_H__
#define __PHY_CONFIG_CHIPS_H__

/* This determines whether a PHY is included or excluded by default */
#ifndef PHY_CONFIG_INCLUDE_CHIP_DEFAULT
#ifdef CDK_CONFIG_INCLUDE_CHIP_DEFAULT
#define PHY_CONFIG_INCLUDE_CHIP_DEFAULT CDK_CONFIG_INCLUDE_CHIP_DEFAULT
#else
#define PHY_CONFIG_INCLUDE_CHIP_DEFAULT 1
#endif
#endif

/* Pick up internel PHY inclusion from CDK */
#if PHY_CONFIG_INCLUDE_CHIP_AUTO == 1
#ifdef CDK_CONFIG_INCLUDE_CHIP_DEFAULT
#include <cdk_config_phys.h>
#endif
#endif

/*
 * BCMGPHY
 */
/* Sets the default include state if it was not given */
#ifndef PHY_CONFIG_INCLUDE_BCMGPHY
#if PHY_CONFIG_INCLUDE_CHIP_AUTO == 1
#define PHY_CONFIG_INCLUDE_BCMGPHY 0
#else
#define PHY_CONFIG_INCLUDE_BCMGPHY PHY_CONFIG_INCLUDE_CHIP_DEFAULT
#endif
#endif

/*
 * BCMI_TSCE_XGXS
 */
/* Sets the default include state if it was not given */
#ifndef PHY_CONFIG_INCLUDE_BCMI_TSCE_XGXS
#if PHY_CONFIG_INCLUDE_CHIP_AUTO == 1
#define PHY_CONFIG_INCLUDE_BCMI_TSCE_XGXS 0
#else
#define PHY_CONFIG_INCLUDE_BCMI_TSCE_XGXS PHY_CONFIG_INCLUDE_CHIP_DEFAULT
#endif
#endif

/*
 * BCMI_VIPER_XGXS
 */
/* Sets the default include state if it was not given */
#ifndef PHY_CONFIG_INCLUDE_BCMI_VIPER_XGXS
#if PHY_CONFIG_INCLUDE_CHIP_AUTO == 1
#define PHY_CONFIG_INCLUDE_BCMI_VIPER_XGXS 0
#else
#define PHY_CONFIG_INCLUDE_BCMI_VIPER_XGXS PHY_CONFIG_INCLUDE_CHIP_DEFAULT
#endif
#endif

#endif /* __PHY_CONFIG_CHIPS_H__ */

/*
 * CONFIG_OPTION Macros. Can be used to determine the build configuration.
 */
 
#ifdef CONFIG_OPTION
CONFIG_OPTION(PHY_CONFIG_INCLUDE_BCMGPHY)
CONFIG_OPTION(PHY_CONFIG_INCLUDE_BCMI_TSCE_XGXS)
CONFIG_OPTION(PHY_CONFIG_INCLUDE_BCMI_VIPER_XGXS)
#undef CONFIG_OPTION
#endif /* #ifdef CONFIG_OPTION */

