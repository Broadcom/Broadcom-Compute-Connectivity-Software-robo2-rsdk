/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 * $Id: $
 *
 * Filename:
 *    mpu.h
 * Description:
 *    Defines MPU attributes in CMSIS terms
 */

#ifndef __MPU_H
#define __MPU_H

#define M7_MPU_XN         (1 << MPU_RASR_XN_Pos) /* Execute Never */
#define M7_MPU_SHAREABLE  (1 << MPU_RASR_S_Pos)  /* Shareable Region */
#define M7_MPU_CACHEABLE  (1 << MPU_RASR_C_Pos)  /* Cacheable Region */
#define M7_MPU_BUFFERABLE (1 << MPU_RASR_B_Pos)  /* Bufferable Region */

/* Access Permissions */
#define M7_MPU_AP_NO_ACCESS  (0x0 << MPU_RASR_AP_Pos) /* No access */
#define M7_MPU_AP_PRIV_RW    (0x1 << MPU_RASR_AP_Pos) /* RW Access from privileged software only */
#define M7_MPU_AP_UNPRIV_RO  (0x2 << MPU_RASR_AP_Pos) /* RW to priv sw, RO for unprivileged software */
#define M7_MPU_AP_ALL_ACCESS (0x3 << MPU_RASR_AP_Pos) /* Full access to all sw */
#define M7_MPU_AP_PRIV_RO    (0x5 << MPU_RASR_AP_Pos) /* RO for privileged sw, No access to unprivleged sw */
#define M7_MPU_AP_READ_ONLY  (0x6 << MPU_RASR_AP_Pos) /* RO Read only, by privileged or unprivileged software */

/* Type Extention */
#define M7_MPU_TEX_DEVICE_MEM (0 << MPU_RASR_TEX_Pos)
#define M7_MPU_TEX_NORMAL_MEM (1 << MPU_RASR_TEX_Pos)
#define M7_MPU_TEX_NORMAL_MEM_NON_CACHEABLE  (6 << MPU_RASR_TEX_Pos)
#define M7_MPU_TEX_NORMAL_MEM_CACHE_RW_ALLOC ((6 << MPU_RASR_TEX_Pos) | (0 << MPU_RASR_C_Pos) | (1 << MPU_RASR_B_Pos))
#define M7_MPU_TEX_NORMAL_MEM_CACHE_WT       ((6 << MPU_RASR_TEX_Pos) | (1 << MPU_RASR_C_Pos) | (0 << MPU_RASR_B_Pos))
#define M7_MPU_TEX_NORMAL_MEM_CACHE_WB       ((6 << MPU_RASR_TEX_Pos) | (1 << MPU_RASR_C_Pos) | (1 << MPU_RASR_B_Pos))

#define M7_MPU_STRONGLY_ORDERED_MEM ((0 << MPU_RASR_TEX_Pos) | (0 << MPU_RASR_C_Pos) | (0 << MPU_RASR_B_Pos))

#endif
