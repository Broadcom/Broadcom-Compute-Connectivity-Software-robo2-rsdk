/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright Broadcom Inc.
 *
 */
#ifndef __QSPI_FLASH_DEF_H__
#define __QSPI_FLASH_DEF_H__

#define SPI_SPANSION_MANUFACTURE_ID     0x01
#define SPI_CYPRESS_MANUFACTURE_ID      0x01
#define SPI_ATMEL_MANUFACTURE_ID        0x1F
#define SPI_ST_MANUFACTURE_ID           0x20
#define SPI_MICRON_MANUFACTURE_ID       0x20
#define SPI_MACRONIX_MANUFACTURE_ID     0xC2
#define SPI_WINBOND_MANUFACTURE_ID      0xEF

/*======================= Internal ======================*/
// defines for saving space
//#ifndef LIB_SPI_FLASH
//#define SAVE_SPACE
//#endif

#define ADDRESS23_16(addr) ((addr >> 16) & 0xFF)
#define ADDRESS15_8(addr)  ((addr >>  8) & 0xFF)
#define ADDRESS7_0(addr)   ((addr) & 0xFF)


#define QSPI_CMD_PAGE_PROGRAM           0x02
#define QSPI_CMD_READ                   0x03
#define QSPI_CMD_WRDI                   0x04 /* reset write disable */
#define QSPI_CMD_RDSR                   0x05 /* read status register */
#define QSPI_CMD_WREN                   0x06 /* set write enable */
#define QSPI_CMD_FAST_READ              0x0B
#define QSPI_CMD_DEEP_POWER_DOWN_RL     0xAB
#define QSPI_CMD_DEEP_POWER_DOWN        0xB9
#define QSPI_CMD_ERASE_CHIP             0xC7
#define QSPI_CMD_ERASE_SECTOR           0xD8
#define QSPI_CMD_ERASE_SUBSECTOR        0x20
#define QSPI_CMD_RDID                   0x9F

/*----- ATMEL definitions -----*/

#define SPI_ATMEL_CMD_READ              0xE8
#define SPI_ATMEL_CMD_READ_TO_BUFFER    0x53
#define SPI_ATMEL_CMD_RDID              QSPI_CMD_RDID
#define SPI_ATMEL_CMD_RDSR              0xD7 /* read status register */
#define SPI_ATMEL_CMD_WDSR              0x01 /* write status register */
#define SPI_ATMEL_CMD_WRITE             0x84
#define SPI_ATMEL_CMD_PAGE_WRITE        0x83
#define SPI_ATMEL_CMD_PAGE_PROGRAM      0x82
#define SPI_ATMEL_CMD_ERASE_SECTOR      0x7C
#define SPI_ATMEL_CMD_ERASE_BLOCK       0x50
#define SPI_ATMEL_CMD_ERASE_PAGE        0x81
#define SPI_ATMEL_CMD_ERASE_CHIP_1      QSPI_CMD_ERASE_CHIP
#define SPI_ATMEL_CMD_ERASE_CHIP_2      0x94
#define SPI_ATMEL_CMD_ERASE_CHIP_3      0x80
#define SPI_ATMEL_CMD_ERASE_CHIP_4      0x9A

#define SPI_ATMEL_STATUS_READY          0x80
#define SPI_ATMEL_PAGE_256_CHECK        0x01

/*======================================================================================================== 
 * For 16MBit ATMEL FLASH MODEL, the PAGE sizes are 528 and 512 bytes instead of 264 and 256 respectively. 
 * Following changes are needed to take care of the difference 
 * ======================================================================================================*/

#define BUFFERED_FLASH_PAGE_SIZE        528
#define BUFFERED_FLASH_PHY_PAGE_SIZE    1024
#define BUFFERED_FLASH_PAGE_POS         10

#define BUFFERED_FLASH_PAGE_SIZE_8M     264
#define BUFFERED_FLASH_PHY_PAGE_SIZE_8M 512
#define BUFFERED_FLASH_PAGE_POS_8M      9

#define BUFFERED_FLASH_BYTE_ADDR_MASK ((1<<BUFFERED_FLASH_PAGE_POS) - 1)
#define BUFFERED_FLASH_BYTE_ADDR_MASK_8M ((1<<BUFFERED_FLASH_PAGE_POS_8M) - 1)
//page_addr = (phy_address & ~BUFFERED_FLASH_BYTE_ADDR_MASK);
//buf_addr  = (phy_address & BUFFERED_FLASH_BYTE_ADDR_MASK);

/*========================================================================================================== 
 * For 16MBit ATMEL FLASH MODEL, the PAGE sizes are 528 and 512 bytes instead of 264 and 256 respectively. 
 * Following changes are needed to take care of the difference 
 * In SW, IF only SPI_ATMEL_161 is defined , it will take care of SPI_ATMEL_321 scenario as the PAGE SIZE is same
 * =========================================================================================================*/
#ifdef PAGE_264B
#define SPI_ATMEL_PAGE_SIZE         528
#define SPI_ATMEL_PAGE_SIZE_8M      264
#else
#define SPI_ATMEL_PAGE_SIZE         512
#define SPI_ATMEL_PAGE_SIZE_8M      256
#endif



/*----- Spansion definitions -----*/

#define SPI_SPANSION_CMD_READ           QSPI_CMD_READ
#define SPI_SPANSION_CMD_FAST_READ      QSPI_CMD_FAST_READ
#define SPI_SPANSION_CMD_WREN       QSPI_CMD_WREN /* set write enable */
#define SPI_SPANSION_CMD_WRDI       QSPI_CMD_WRDI /* reset write enable */
#define SPI_SPANSION_CMD_RDID           QSPI_CMD_RDID
#define SPI_SPANSION_CMD_RDSR           0x06 /* read status register */
#define SPI_SPANSION_CMD_PAGE_PROGRAM   QSPI_CMD_PAGE_PROGRAM
#define SPI_SPANSION_CMD_PAGE_WRITE     0x0A
#define SPI_SPANSION_CMD_PAGE_ERASE     0xDB
#define SPI_SPANSION_CMD_ERASE_SECTOR   QSPI_CMD_ERASE_SECTOR
#define SPI_SPANSION_CMD_ERASE_CHIP     QSPI_CMD_ERASE_CHIP



/*----- ST Micro definitions -----*/

#define SPI_ST_PAGE_SIZE        256
#define SPI_WB_PAGE_SIZE        256
#define SPI_MX_PAGE_SIZE        256

#define SPI_ST_PAGES_PER_SEC    256
#define SPI_ST_SECTOR_SIZE      (SPI_ST_PAGE_SIZE * SPI_ST_PAGES_PER_SEC)
#define SPI_ST_SUBSECTOR_SIZE   (4096)

#define SPI_ST_STATUS_BUSY      0x01
#define SPI_ST_STATUS_WEN       0x02
#define SPI_ST_STATUS_BP0       0x04
#define SPI_ST_STATUS_BP1       0x08
#define SPI_ST_STATUS_BPL       0x80

#define SPI_WB_STATUS_BUSY      0x01
#define SPI_WB_STATUS_WEN       0x02
#define SPI_WB_STATUS_BP0       0x04
#define SPI_WB_STATUS_BP1       0x08
#define SPI_WB_STATUS_BP2       0x10
#define SPI_WB_STATUS_TB        0x20 /* Top-Bottom Write Protect */
#define SPI_WB_STATUS_SRP       0x80 /* Status register protect */

#define SPI_MX_STATUS_BUSY      0x01 /* WIP */
#define SPI_MX_STATUS_WEN       0x02 /* Write Enable Latch */
#define SPI_MX_STATUS_BP0       0x04
#define SPI_MX_STATUS_BP1       0x08
#define SPI_MX_STATUS_BP2       0x10
#define SPI_MX_STATUS_BP3       0x20
#define SPI_MX_STATUS_CP        0x40 /* continuous program mode */
#define SPI_MX_STATUS_SRWD      0x80 /* status register write protect */


#define SPI_ST_CMD_READ                 QSPI_CMD_READ
#define SPI_ST_CMD_FAST_READ            QSPI_CMD_FAST_READ
#define SPI_ST_CMD_WREN                 QSPI_CMD_WREN /* set write enable */
#define SPI_ST_CMD_WRDI                 QSPI_CMD_WRDI /* reset write enable */
#define SPI_ST_CMD_RDID                 QSPI_CMD_RDID
#define SPI_ST_CMD_RDSR                 QSPI_CMD_RDSR /* read status register */
#define SPI_ST_CMD_PAGE_PROGRAM         QSPI_CMD_PAGE_PROGRAM
#define SPI_ST_CMD_PAGE_WRITE           0x0A
#define SPI_ST_CMD_PAGE_ERASE           0xDB
#define SPI_ST_CMD_ERASE_SECTOR         QSPI_CMD_ERASE_SECTOR
#define SPI_ST_CMD_ERASE_SUBSECTOR      QSPI_CMD_ERASE_SUBSECTOR
#define SPI_ST_CMD_ERASE_CHIP           QSPI_CMD_ERASE_CHIP
#define SPI_ST_CMD_DEEP_POWER_DOWN      QSPI_CMD_DEEP_POWER_DOWN
#define SPI_ST_CMD_DEEP_POWER_DOWN_RL   QSPI_CMD_DEEP_POWER_DOWN_RL



/*----- Winbond definitions -----*/

#define SPI_WB_CMD_READ                 QSPI_CMD_READ
#define SPI_WB_CMD_FAST_READ            QSPI_CMD_FAST_READ
#define SPI_WB_CMD_WREN                 QSPI_CMD_WREN /* set write enable */
#define SPI_WB_CMD_WRDI                 QSPI_CMD_WRDI /* reset disable */
#define SPI_WB_CMD_RDID                 QSPI_CMD_RDID
#define SPI_WB_CMD_WRSR                 0x01 /* write status register */
#define SPI_WB_CMD_RDSR                 QSPI_CMD_RDSR /* read status register */
#define SPI_WB_CMD_PAGE_PROGRAM         QSPI_CMD_PAGE_PROGRAM
#define SPI_WB_CMD_ERASE_BLK            QSPI_CMD_ERASE_SECTOR
#define SPI_WB_CMD_ERASE_SECTOR         0x20
#define SPI_WB_CMD_ERASE_CHIP           QSPI_CMD_ERASE_CHIP
#define SPI_WB_CMD_DEEP_POWER_DOWN      QSPI_CMD_DEEP_POWER_DOWN
#define SPI_WB_CMD_DEEP_POWER_DOWN_RL   QSPI_CMD_DEEP_POWER_DOWN_RL



/*----- Macronix definitions -----*/

#define SPI_MX_CMD_READ                 QSPI_CMD_READ
#define SPI_MX_CMD_FAST_READ            QSPI_CMD_FAST_READ
#define SPI_MX_CMD_WREN                 QSPI_CMD_WREN /* set write enable */
#define SPI_MX_CMD_WRDI                 QSPI_CMD_WRDI /* reset disable */
#define SPI_MX_CMD_RDID                 QSPI_CMD_RDID
#define SPI_MX_CMD_WRSR                 0x01 /* write status register */
#define SPI_MX_CMD_RDSR                 QSPI_CMD_RDSR /* read status register */
#define SPI_MX_CMD_PAGE_PROGRAM         QSPI_CMD_PAGE_PROGRAM
#define SPI_MX_CMD_CONT_PROGRAM         0xAD
#define SPI_MX_CMD_ERASE_BLK            QSPI_CMD_ERASE_SECTOR
#define SPI_MX_CMD_ERASE_SECTOR         0x20
#define SPI_MX_CMD_ERASE_CHIP           QSPI_CMD_ERASE_CHIP
#define SPI_MX_CMD_ERASE_CHIP_OPT       0x60 /* Optional chip erase 0x60/0xc7, we will use 0xc7 */
#define SPI_MX_CMD_DEEP_POWER_DOWN      QSPI_CMD_DEEP_POWER_DOWN
#define SPI_MX_CMD_DEEP_POWER_DOWN_RL   QSPI_CMD_DEEP_POWER_DOWN_RL

/*----- Additional Macronix definitions -----*/

#define SPI_MX_CMD_REMS     0x90 /* Read Electronic manu. & device ID */
#define SPI_MX_CMD_REMS2    0xEF /* Read ID for 2x I/O mode */
#define SPI_MX_CMD_ENSO     0xB1 /* Enter secured OTP */
#define SPI_MX_CMD_EXSO     0xC1 /* Exit secured OTP */
#define SPI_MX_CMD_RDSCUR   0x2B /* Read security register */
#define SPI_MX_CMD_WRSCUR   0x2F /* Write security register */
#define SPI_MX_CMD_ESRY     0x70 /* Enable S0 to output RY/BY# */
#define SPI_MX_CMD_DSRY     0x80 /* Disable S0 to output RY/BY# */

#endif
