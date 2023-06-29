/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *    uart_spi.h
 * Description:
 *    Contains UART wrapper defintions required for communinication over
 *    UART-SPI interface to the Avenger
 */

#ifndef __ROBO2_UART_H
#define __ROBO2_UART_H


#define USER_UART_SPI_DEVICE_NAME   "/dev/ttyS3"
#define USER_UART_SPI_BAUD_RATE      9600
#define MAX_UART_SPI_DEVICES         1
#define MAX_UART_SPI_RETRIES         100
#define MAX_UART_SPI_RETRY_DELAY     100 /* in micro seconds */

/*
 * Function:
 *    soc_uart_spi_init
 * Purpose:
 *    Initialize UART wrapper interface
 * Parameters:
 *    unit - unit number
 * Returns:
 *    0 - Success
 *   -1 - Failure
 */
extern int
soc_uart_spi_init(int unit);

/*
 * Function:
 *    soc_uart_spi_terminate
terminate
 * Purpose:
 *    Close UART wrapper interface
 * Parameters:
 *    unit - unit number
 * Returns:
 *    0 - Success
 *   -1 - Failure
 */
extern int
soc_uart_spi_terminate(int unit);

/*
 * Function:
 *    soc_uart_spi_read
 * Purpose:
 *    Read a ROBO2 SPI register using UART interface
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
soc_uart_spi_read(int unit, uint32 reg, uint8 *buffer, int len);

/*
 * Function:
 *    soc_uart_spi_write
 * Purpose:
 *    Write to a ROBO2 SPI register using UART interface
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
soc_uart_spi_write(int unit, uint32 reg, uint8 *buffer, int len);

#endif /* !__ROBO2_UART_SPI_H */
