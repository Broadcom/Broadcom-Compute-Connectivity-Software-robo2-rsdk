/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *    qspi.h
 * Description:
 *    QSPI driver
 *
 */
#ifndef __QSPI_H__
#define __QSPI_H__

#include <sal_types.h>

/* Controller-specific definitions: */

/* SPI mode flags */
#define SPI_CPHA    0x01            /* clock phase */
#define SPI_CPOL    0x02            /* clock polarity */
#define SPI_MODE_0  (0|0)           /* (original MicroWire) */
#define SPI_MODE_1  (0|SPI_CPHA)
#define SPI_MODE_2  (SPI_CPOL|0)
#define SPI_MODE_3  (SPI_CPOL|SPI_CPHA)
#define SPI_CS_HIGH 0x04            /* CS active high */
#define SPI_LSB_FIRST   0x08        /* per-word bits-on-wire */
#define SPI_3WIRE   0x10            /* SI/SO signals shared */
#define SPI_LOOP    0x20            /* loopback mode */

/* SPI transfer flags */
#define SPI_XFER_BEGIN  0x01        /* Assert CS before transfer */
#define SPI_XFER_END    0x02        /* Deassert CS after transfer */


/*-----------------------------------------------------------------------
 * Initialization, must be called once on start up.
 *
 */
int qspi_init(void);


/*-----------------------------------------------------------------------
 * Claim the bus and prepare it for communication with a given slave.
 *
 * This must be called before doing any transfers with a SPI slave. It
 * will enable and initialize any SPI hardware as necessary, and make
 * sure that the SCK line is in the correct idle state. It is not
 * allowed to claim the same bus for several slaves without releasing
 * the bus in between.
 *
 *   slave: The SPI slave
 *
 * Returns: 0 if the bus was claimed successfully, or a negative value
 * if it wasn't.
 */
int qspi_claim_bus(void);

/*-----------------------------------------------------------------------
 * Release the SPI bus
 *
 * This must be called once for every call to spi_claim_bus() after
 * all transfers have finished. It may disable any SPI hardware as
 * appropriate.
 *
 *   slave: The SPI slave
 */
void qspi_release_bus(void);

/* cmd_len bytes cmd & data_len to read out */
int qspi_cmd_read(const uint8_t *cmd,
                  int cmd_len, void *data, int data_len);

/* cmd_len bytes cmd & data_len to write */
int qspi_cmd_write(const uint8_t *cmd, int cmd_len,
                   const void *data, int data_len);

/* 1byte cmd & need response */
int qspi_cmd(uint8_t cmd, void *response, int len);

int qspi_bspi_quad(void);

int qspi_bspi_dual(void);

int qspi_bspi_single(void);

#define debug(...)

#endif
