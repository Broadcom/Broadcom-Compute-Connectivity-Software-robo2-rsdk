/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *    spi.h
 * Description:
 *    Contains Avenger SPI defintions
 */

#ifndef __ROBO2_SPI_H
#define __ROBO2_SPI_H

#ifndef SPI_MODE_0
#define SPI_MODE_0     0    /* CPOL=0, CPHA=0 */
#define SPI_MODE_1     1    /* CPOL=0, CPHA=1 */
#define SPI_MODE_2     2    /* CPOL=1, CPHA=0 */
#define SPI_MODE_3     3    /* CPOL=1, CPHA=1 */
#endif

/* Set to mode 3 - default for avenger b0 */
#define ROBO2_SPI_MODE         SPI_MODE_3

typedef enum {
    ROBO2_SPI_READ_REQ       = 0,
    ROBO2_SPI_READ_STATUS    = 1,
    ROBO2_SPI_READ_DATA      = 2,
    ROBO2_SPI_WRITE_REQUEST  = 4,
    ROBO2_SPI_GET_WRITE_STS  = 5,
    ROBO2_SPI_FAST_READ      = 6,
    ROBO2_SPI_FAST_WRITE     = 7,
    ROBO2_SPI_GET_SPI_STATUS = 8,
    ROBO2_SPI_CLR_SPI_STATUS = 9,
    ROBO2_SPI_RESET_SPI_BLK  = 10,
    ROBO2_SPI_RESET_CHIP     = 11
} robo2_spi_cmd_t;

typedef enum {
    ROBO2_SPI_WRITE_OVF          = 0x0001,
    ROBO2_SPI_WRITE_ABORT        = 0x0002,
    ROBO2_SPI_WRITE_FM_OVF       = 0x0004,
    ROBO2_SPI_WRITE_FM_ABRT      = 0x0008,
    ROBO2_SPI_WRITE_AXI_SLV_ERR  = 0x0010,
    ROBO2_SPI_WRITE_AXI_DEC_ERR  = 0x0020,
    ROBO2_SPI_READ_OVF           = 0x0100,
    ROBO2_SPI_READ_ABORT         = 0x0200,
    ROBO2_SPI_READ_FM_OVF        = 0x0400,
    ROBO2_SPI_READ_FM_ABRT       = 0x0800,
    ROBO2_SPI_READ_AXI_SLV_ERR   = 0x1000,
    ROBO2_SPI_READ_AXI_DEC_ERR   = 0x2000,
} robo2_spi_status_t;

#define ROBO2_L2BLEN(len) ((((len) >> 2) - 1) & 0x7)

#define ROBO2_SPI_BURST_COMMAND(cmd, len) \
             (((cmd) << 4) | (ROBO2_L2BLEN(len) << 1))

#define ROBO2_SPI_COMMAND(cmd) ((cmd) << 4)

#define ROBO2_SPI_TRANS_STS_IDLE         0x0
#define ROBO2_SPI_TRANS_STS_PROGRESS     0x1
#define ROBO2_SPI_TRANS_STS_COMPLETE     0x2
#define ROBO2_SPI_TRANS_STS_ERROR        0x3
#define ROBO2_SPI_TRANS_STS_MASK         0x3

extern int
spi_status_get(int dev, uint16 *status);

/*
 * Function:
 *    soc_spi_init
 * Purpose:
 *    Initialize SPI wrapper interface
 * Parameters:
 *    unit - unit number
 * Returns:
 *    0 - Success
 *   -1 - Failure
 */
extern int
soc_spi_init(int unit);

/*
 * Function:
 *    soc_spi_terminate
 * Purpose:
 *    Close SPI wrapper interface
 * Parameters:
 *    unit - unit number
 * Returns:
 *    0 - Success
 *   -1 - Failure
 */
extern int
soc_spi_terminate(int unit);

/*
 * Function:
 *    soc_spi_read
 * Purpose:
 *    Read a ROBO2 SPI register
 * Parameters:
 *    unit - unit number
 *    reg  - address of register
 *    buffer - pointer data buffer
 *    len  - len of data to read
 * Returns:
 *    0 - Success
 *   -1 - Failure
 */
extern int
soc_spi_read(int unit, uint32 reg, uint8 *buffer, int len);

/*
 * Function:
 *    soc_spi_write
 * Purpose:
 *    Write to a ROBO2 SPI register
 * Parameters:
 *    unit - unit number
 *    reg  - address of register
 *    buffer - pointer data buffer
 *    len  - len of data to write
 * Returns:
 *    0 - Success
 *   -1 - Failure
 */
extern int
soc_spi_write(int unit, uint32 reg, uint8 *buffer, int len);

#endif /* !__ROBO2_SPI_H */
