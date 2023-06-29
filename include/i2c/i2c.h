
/*
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * i2c.h
 *
 * i2c header file.
 */

#ifndef __I2C_H__
#define __I2C_H__
#include <sal_types.h>


/*
 * @brief   I2C SDA mux
 * @details I2C SDA selection.
 */
typedef enum {
    I2C_SDA8,
    I2C_SDA9,
    I2C_SDA10,
    I2C_SDA11,
    I2C_SDA12,
    I2C_SDA13,
    I2C_SDA_GPHY,
} I2C_MUX_SEL;

/*
 * @brief      I2C master Initialization.
 * @details    Must be called once on start up to initialize controller.
 * @retval     0:OK
 * @retval     -1:ERROR
 */
int32 i2c_master_init (int32 unit);

/*
 * @brief      I2C master Probe.
 * @details    Probe the given I2C slave device address.
 * @param[in]  unit unit number.
 * @param[in]  chip Chip address.
 * @retval     0:OK
 * @retval     -1:ERROR
 */
int32 i2c_master_probe (int32 unit, uint8_t chip);

/**
 * @brief      I2C master read.
 * @details    Read data from specified chip and address.
 * @param[in]  unit unit number.
 * @param[in]  mux SDA line select.
 * @param[in]  chip   I2C chip address, range 0...127.
 * @param[out] buffer Where to read the data.
 * @param[in]  length How many bytes to read.
 * @retval     0:OK
 * @retval     -1:ERROR
 */
int32 i2c_master_read (int32 unit, I2C_MUX_SEL mux, uint8_t chip,
                       uint8_t *buffer, uint32 length);

/**
 * @brief      I2C master write.
 * @details    Write data to specified chip and address.
 * @param[in]  unit unit number.
 * @param[in]  mux SDA line select.
 * @param[in]  chip   I2C chip address, range 0...127.
 * @param[out] buffer Buffer to write.
 * @param[in]  length How many bytes to write.
 * @retval     0:OK
 * @retval     -1:ERROR
 */
int32 i2c_master_write (int32 unit, I2C_MUX_SEL mux, uint8_t chip,
                        uint8_t *buffer, uint32 length);
#endif
