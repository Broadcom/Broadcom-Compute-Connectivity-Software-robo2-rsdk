/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * File:
 *    init.h
 * Description:
 *    Definitions
 */

#ifdef BCM_53158_A0

#include <soc/robo2/bcm53158/config.h>

#ifdef CORTEX_M7
#ifdef VERBOSE_REGSET
#define SET_REG(r, v)  sysprintf("Setting %08x to %08x\n", (r), (v)); (*(volatile uint32*)(r)) = (v)
#else
#define SET_REG(r, v)  (*(volatile uint32*)(r)) = (v)
#endif
#define GET_REG(r)     (*(volatile uint32*)(r)) 
#endif /* CORTEX_M7 */

#define BCM53158_A0_CRU_SYSRESET_R1_REG      0x402000B4
#define BCM53158_A0_CRU_SYSRESET_R2_REG      0x402000B8
#define BCM53158_A0_CRU_SYSRESET_R3_REG      0x402000BC
#define BCM53158_A0_CRU_SYSRESET_R4_REG      0x402000C0
#define BCM53158_A0_CRU_SYSRESET_R5_REG      0x402000C4
#define BCM53158_A0_CRU_STRAP_RAW_REG        0x40200108
#define BCM53158_A0_CRU_STRAP_STS_REG        0x4020010c
#define BCM53158_A0_CRU_CHIP_ID_REG          0x402001A0
#define BCM53158_A0_CRU_CHIP_REVID_REG       0x402001A4
#define BCM53158_A0_CRU_CHIP_MODID_REG       0x402001A8

int set_qspi_flash_mpu(uint32_t size);
int bcm53158_early_init(void);
int bcm53158_core_init(void);
int bcm53158_chip_init(void);

#ifdef CORTEX_M7
extern void sysprint_setup(uint32 baudrate);
extern void sysprint(char *);
extern void sysprintf(char *,...);
extern void sysdelay(int usecs);
extern void sysgetdevid(int dev, uint16 *dev_id, uint8 *rev_id);
extern void get_device_id(int unit, uint32 *chipid, uint32 *revid, uint32 *modelid);
extern void wait_for_debugger(void);
#endif /* CORTEX_M7 */


#endif /* BCM_53158_A0 */

