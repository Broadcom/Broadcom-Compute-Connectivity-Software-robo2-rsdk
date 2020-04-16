/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 *  File:
 *      qspi_flash.h
 *  Description:
 *      QSPI Flash header file
 *
 */

#ifndef __QSPI_FLASH_H__
#define __QSPI_FLASH_H__

/**
 * @brief   SFLASH type
 * @details SFLASH type
 */
typedef enum {
    SERIAL_FLASH_UNKNOWN        = 0x00, ///< type unknown
    SERIAL_FLASH_ATMEL          = 0x01, ///< type atmel
    SERIAL_FLASH_CYPRESS        = 0x02, ///< type spansion/cypress
    SERIAL_FLASH_ST             = 0x03, ///< type st
    SERIAL_FLASH_MICRON         = 0x04, ///< type st
    SERIAL_FLASH_WINBOND        = 0x05, ///< type winbond
    SERIAL_FLASH_MACRONIX       = 0x06, ///< type macronix
    SERIAL_FLASH_ATMEL_8M       = 0x07, ///< type atmel 8M
} sf_type;

/**
 * @brief   Flashinfo
 * @details Serial flash info
 */
typedef struct {
    sf_type flash_type;
    uint32  flash_size;   /* Mbits */
    uint32  sector_size;
    uint32  page_size;
} sflashinfo_t;

#define FLASH_SIZE_MB(size) ((size) << 17)

/**
 * @brief       Initialize params from enumeration done externally
 * @param[in]  flashinfo, pointer to flash info
 * @retval      Nothing
 */
void qspi_flash_initialize(sflashinfo_t *flashinfo);

/**
 * @brief       sflash enumerate
 * @details     sflash enumerate, get the type of sflash
 * @param[out]  flashinfo Optional pointer to flash info
 * @retval      0:OK
 * @retval      -1:ERROR
 */
int qspi_flash_enumerate(sflashinfo_t *flashinfo);

/**
 * @brief       SFLASH read
 * @details     SFLASH read
 * @param[in]   address         begin address to read
 * @param[out]  data            output buffer
 * @param[in]   data_len        length to read
 * @retval      0:OK
 * @retval      -1:ERROR
 */
int soc_qspi_flash_read(uint32 address, uint8 *data, uint32 data_len);

/**
 * @brief       SFLASH erase
 * @details     SFLASH erase. erase unit is section.
 * @param[in]   address         begin address to erase, could be any address with section.
 * @param[in]   len             length to erase
 * @retval      0:OK
 * @retval      -1:ERROR
 */
int soc_qspi_flash_erase(uint32 address, uint32 len);

/**
 * @brief       SFLASH write
 * @details     SFLASH write. sflash must be erased before write
 * @param[in]   address         begin address to write
 * @param[in]   data            input buffer
 * @param[in]   data_len        length to write
 * @retval      0:OK
 * @retval      -1:ERROR
 */
int soc_qspi_flash_write(uint32 address, uint8 *data, uint32 data_len);


/**
 * @brief       SFLASH whole chip erase
 * @details     SFLASH whole chip erase, will take very long time
 * @retval      0:OK
 * @retval      -1:ERROR
 */
int soc_qspi_flash_chip_erase();


#endif
