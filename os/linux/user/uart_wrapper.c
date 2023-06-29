/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * Filename:
 *    uart_wrapper.c
 * Description:
 *    robo2 UART wrapper for user mode access over SPI.
 */

#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <osal/sal_console.h>
#include <osal/sal_util.h>
#include <osal/sal_time.h>
#include <sal_sync.h>
#include <soc/drv.h>
#include <soc/robo2/bcm53158/init.h>
#include <soc/robo2/bcm53158/spi.h>
#include <soc/robo2/bcm53158/uart_spi.h>

sal_mutex_t uart_spi_mutexid = NULL;
static int uart_spi_status_get(int dev, uint16 *status);

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
/*
 * Table defines which UART interface corresponds to the unit
 */
static int uart_dev[2] = {
    0,                        /* Unit 0 */
    0                         /* Unit 1, accessed via Unit 0 */
};

static struct {
    int    uart_fd; /* UART device descriptor */
} sys_uart_devs[MAX_UART_SPI_DEVICES];

char *uart_spi_status[] = {
    "spi status ok"
    "write overflow"
    "write_abort"
    "write fm overflow"
    "write fm abort"
    "write axi slave error"
    "write axi decode error"
    "read overflow"
    "read abort"
    "read fm overflow"
    "read fm abort"
    "read axi slave error"
    "read axi decode error"
};

char *
uart_spi_errmsg(int code)
{
    if ((code >= 0) && (code < sizeof(uart_spi_status)/sizeof(char*))) {
        return uart_spi_status[code];
    } else {
        return "UART spi transaction failed";
    }
}

/*
 * Function
 *    uart_read
 * Purpose
 *    Wrapper routine to read the bytes
 * Parameters
 *    fd - file descriptor
 *    buf - receive buffer
 *    len - number of bytes
 * Returns:
 *    0 - Success
 *   -1 - Failed
 */
int
uart_read(int fd, uint8* buf, uint32 len)
{
    int bytes_read;
    uint32 bytes_to_read = len, bytes_cnt = 0;
    uint8 retry_cnt = 0;

    do {
        bytes_read = read(fd, &buf[bytes_cnt], bytes_to_read);
        if (bytes_read != -1 ) {
            bytes_cnt += bytes_read;
        } else {
            sal_printf("UART read error: %d", bytes_read);
            return -1;
        }
        if (bytes_cnt == len){
            return 0;
        }
        bytes_to_read = len - bytes_cnt;
        retry_cnt++;
        sal_usleep(MAX_UART_SPI_RETRY_DELAY);
    } while (retry_cnt < MAX_UART_SPI_RETRIES);
    sal_printf("UART read error: bytes read %d expected %d\n",
                bytes_cnt, len);
    return -1;
}

/*
 * Function
 *    uart_read_till_empty
 * Purpose
 *    Wrapper routine to read all the bytes from receive buffer.
 * Parameters
 *    fd - file descriptor
 *    buf - receive buffer
 *    len - number of bytes
 * Returns:
 *    0 - Success
 *   -1 - Failed
 */
int
uart_read_till_empty(int fd, uint8* buf, uint32 len)
{
    int bytes_read = 0;

    do {
        bytes_read = read(fd, buf, len);
        if (bytes_read == -1) {
            sal_printf("UART read error: %d", bytes_read);
            return -1;
        }
    } while (bytes_read);
    return 0;
}

/*
 * Function
 *    uart_spi_wait_for_read
 * Purpose
 *   Poll the SPI slave till slave is ready for read data to be delivered
 *   using UART interface
 * Parameters
 *    dev - device number
 *    status - (out) spi status of last read transaction
 * Returns:
 *    0 - Success
 *   -1 - Failed
 */
int
uart_spi_wait_for_read(int dev, uint16 *status)
{
    int i, cnt;
    int fd = sys_uart_devs[dev].uart_fd;
    uint8 tx[6];
    uint8 rx[4];
    uint8 rx_buf[5];
    uint16 sts;
    int bytes_read = 0;
    int bytes_written = 0;

    tx[0] = ARRAY_SIZE(tx) - 1;
    tx[1] = ROBO2_SPI_COMMAND(ROBO2_SPI_READ_STATUS);
    cnt = 0;
    do {
        /* Before writing read the data to make sure buffer is empty */
        if (uart_read_till_empty(fd, rx, ARRAY_SIZE(rx)) != 0) {
            return -1;
        }
        bytes_written = write(fd, tx, ARRAY_SIZE(tx));
        if (bytes_written != (ARRAY_SIZE(tx))) {
            sal_printf("UART write error: bytes written %d\n",
                        bytes_written);
            return -1;
        }
        if (uart_read(fd, rx_buf, ARRAY_SIZE(rx_buf)) != 0) {
            return -1;
        }
        for (i = 0; i < bytes_read; i++) {
            rx[i] = rx_buf[1 + i];
        }
        for (i = 0; i < ARRAY_SIZE(rx); i+=2) {
            sts = ((rx[i] << 8) | rx[i+1]);
            sts = sts & ROBO2_SPI_TRANS_STS_MASK;
            if (sts  == ROBO2_SPI_TRANS_STS_COMPLETE) {
                *status= sts;
                return 0;
            } else if (sts == ROBO2_SPI_TRANS_STS_ERROR) {
                break;
            }
        }
        sal_usleep(100);
        if (++cnt > MAX_UART_SPI_RETRIES) { break;}
    } while (sts == ROBO2_SPI_TRANS_STS_PROGRESS);
    uart_spi_status_get(dev, &sts);
    if (status) *status= sts;
    return -1;
}

/*
 * Function
 *    uart_spi_wait_for_write
 * Purpose
 *   Poll the SPI slave till slave has processed all write data,
 *   using UART interface
 * Parameters
 *    dev - device number
 *    status - (out) spi status of last write operation
 * Returns:
 *    0 - Success
 *   -1 - Failed
 */
int
uart_spi_wait_for_write(int dev, uint16 *status)
{
    int i, cnt;
    int fd = sys_uart_devs[dev].uart_fd;
    uint8 tx[6];
    uint8 rx[4];
    uint8 rx_buf[5];
    uint16 sts;
    int bytes_read = 0;
    int bytes_written = 0;

    tx[0] = ARRAY_SIZE(tx) - 1;
    tx[1] = ROBO2_SPI_COMMAND(ROBO2_SPI_GET_WRITE_STS);
    cnt = 0;
    do {
        /* Before writing read the data to make sure buffer is empty */
        if (uart_read_till_empty(fd, rx, ARRAY_SIZE(rx)) != 0) {
            return -1;
        }
        bytes_written = write(fd, tx, ARRAY_SIZE(tx));
        if (bytes_written != ARRAY_SIZE(tx)) {
            sal_printf("UART write error: bytes written %d\n",
                        bytes_written);
            return -1;
        }
        if (uart_read(fd, rx, ARRAY_SIZE(rx_buf)) != 0) {
            return -1;
        }
        for (i = 0; i < bytes_read; i++) {
            rx[i] = rx_buf[1 + i];
        }
        for (i = 0; i < ARRAY_SIZE(rx); i+=2) {
            sts = ((rx[i] << 8) | rx[i+1]);
            sts = sts & ROBO2_SPI_TRANS_STS_MASK;
            if (sts == ROBO2_SPI_TRANS_STS_COMPLETE) {
                *status= sts;
                return 0;
            } else if (sts == ROBO2_SPI_TRANS_STS_ERROR) {
                break;
            }
        }
        sal_usleep(100);
        if (++cnt > MAX_UART_SPI_RETRIES) { break;}
    } while (sts == ROBO2_SPI_TRANS_STS_PROGRESS);
    uart_spi_status_get(dev, &sts);
    if (status) *status= sts;
    return -1;
}

/*
 * Function:
 *    uart_spi_dev_read
 * Purpose:
 *    Routine to send spi read message for Robo2 devices using UART interface.
 * Parameters:
 *    dev - device number
 *    reg - register addr to read
 *    buf - (out) buffer addr to store the reading result
 *    len - number bytes to read
 * Returns:
 *    robo2_spi_status_t
 */
static robo2_spi_status_t
uart_spi_dev_read(int dev, uint32 reg, uint8 *buf, int len)
{
    robo2_spi_status_t ret;
    int fd = sys_uart_devs[dev].uart_fd;
    uint8 tx[32];
    uint8 rx[32];
    uint16 status;
    int bytes_written = 0;
    uint8 cnt;

    /* Before writing read the data to make sure buffer is empty */
    if (uart_read_till_empty(fd, rx, ARRAY_SIZE(rx)) != 0) {
        return -1;
    }
    reg = sal_htonl(reg);
    tx[0] = 5; /* length of the transmit data bytes */
    tx[1] = ROBO2_SPI_COMMAND(ROBO2_SPI_READ_REQ);
    tx[2] = (reg >> 24) & 0xff;
    tx[3] = (reg >> 16) & 0xff;
    tx[4] = (reg >>  8) & 0xff;
    tx[5] = reg & 0xff;

    bytes_written = write(fd, tx, 6);
    if (bytes_written != 6) {
        sal_printf("UART write error: bytes written %d\n", bytes_written);
        return -1;
    }

    ret = uart_spi_wait_for_read(dev, &status);
    if (ret < 0) {
        sal_printf("UART spi wait for read failed\n");
        return status;
    }

    /* Before writing read the data to make sure buffer is empty */
    if (uart_read_till_empty(fd, rx, ARRAY_SIZE(rx)) != 0) {
        return -1;
    }
    tx[0] = len + 1; /* length of the transmit data bytes */
    tx[1] = ROBO2_SPI_COMMAND(ROBO2_SPI_READ_DATA);
    for (cnt = 0; cnt < len; cnt++) {
        tx[2 + cnt] = 0x0; /* dummy data */
    }
    bytes_written = write(fd, tx, (len + 2));
    if (bytes_written != (len + 2)) {
        sal_printf("UART write error: bytes written %d\n", bytes_written);
        return -1;
    }
    /* read received data */
    if (uart_read(fd, rx, (len + 1)) != 0) {
        return -1;
    }
    for (cnt = 0; cnt < len; cnt++) {
        buf[cnt] = rx[1 + cnt];
    }
    return 0;
}

/*
 * Function:
 *    uart_spi_dev_write
 * Purpose:
 *    Routine to send spi write message for Robo2 devices using UART interface.
 * Parameters:
 *    dev - device number
 *    reg - register addr to write
 *    buf - (out) buffer addr to store the reading result
 *    len - number bytes to write
 * Returns:
 *    robo2_spi_status_t
 */
static robo2_spi_status_t
uart_spi_dev_write(int dev, uint32 reg, uint8 *buf, int len)
{
    robo2_spi_status_t ret;
    int fd = sys_uart_devs[dev].uart_fd;
    uint8 tx[32];
    uint16 status = 0;
    int cnt;
    int bytes_written = 0;

    /* Before writing read the data to make sure buffer is empty */
    if (uart_read_till_empty(fd, tx, ARRAY_SIZE(tx)) != 0) {
        return -1;
    }
    /* Fill the address */
    reg = sal_htonl(reg);
    tx[0] = len + 5;
    tx[1] = ROBO2_SPI_COMMAND(ROBO2_SPI_WRITE_REQUEST);
    tx[2] = (reg >> 24) & 0xff;
    tx[3] = (reg >> 16) & 0xff;
    tx[4] = (reg >>  8) & 0xff;
    tx[5] = reg & 0xff;

    /* Fill the data */
    for (cnt = 0; cnt < len; cnt++) {
        tx[6 + cnt] = buf[cnt];
    }
    bytes_written = write(fd, tx, (len + 6));
    if (bytes_written != (len + 6)) {
        sal_printf("UART write error: bytes written %d\n", bytes_written);
        return -1;
    }
    ret = uart_spi_wait_for_write(dev, &status);
    if (ret < 0) {
        sal_printf("UART spi wait for read failed\n");
        return -1;
    }
    return 0;
}

/*
 * Function:
 *    uart_spi_reset
 * Purpose:
 *    Routine to send spi reset message for Robo2 devices using UART interface.
 * Parameters:
 *    dev - device number
 *    resetall - reset chip or SPI block
 * Returns:
 *    robo2_spi_status_t
 */
int
uart_spi_reset(int dev, int resetall)
{
    uint8 tx[2];
    uint8 rx[1];
    int bytes_written = 0;
    int fd = sys_uart_devs[dev].uart_fd;

    tx[0] = ARRAY_SIZE(tx);
    if (resetall) {
        tx[1] = ROBO2_SPI_COMMAND(ROBO2_SPI_RESET_CHIP);
    } else {
        tx[1] = ROBO2_SPI_COMMAND(ROBO2_SPI_RESET_SPI_BLK);
    }
    /* Before writing read the data to make sure buffer is empty */
    if (uart_read_till_empty(fd, rx, ARRAY_SIZE(rx)) != 0) {
        return -1;
    }
    bytes_written = write(fd, tx, ARRAY_SIZE(tx));
    if (bytes_written != ARRAY_SIZE(tx)) {
        sal_printf("UART write error: bytes written %d\n", bytes_written);
        return -1;
    }
    /* Dummy read */
    if (uart_read(fd, rx, ARRAY_SIZE(rx)) != 0) {
        return -1;
    }
    return 0;
}

/*
 * Function:
 *    uart_spi_status_get
 * Purpose:
 *    Routine to get spi status for Robo2 devices using UART interface.
 * Parameters:
 *    dev - device number
 *    status - SPI status
 * Returns:
 *    robo2_spi_status_t
 */
int
uart_spi_status_get(int dev, uint16 *status)
{
    uint8 tx[4];
    uint8 rx[2];
    uint8 rx_buf[3], i;
    int bytes_read = 0;
    int bytes_written = 0;
    int fd = sys_uart_devs[dev].uart_fd;

    if (!status) {
        return -1;
    }
    /* Before writing read the data to make sure buffer is empty */
    if (uart_read_till_empty(fd, rx, ARRAY_SIZE(rx)) != 0) {
        return -1;
    }
    tx[0] = ARRAY_SIZE(tx);
    tx[1] = ROBO2_SPI_COMMAND(ROBO2_SPI_GET_SPI_STATUS);
    bytes_written = write(fd, tx, ARRAY_SIZE(tx));
    if (bytes_written != ARRAY_SIZE(tx)) {
        sal_printf("UART write error: bytes written %d\n", bytes_written);
        return -1;
    }
    if (uart_read(fd, rx_buf, ARRAY_SIZE(rx_buf)) != 0) {
        return -1;
    }
    for (i = 0; i < bytes_read; i++) {
        rx[i] = rx_buf[1 + i];
    }
    *status = ((rx[0] << 8) | rx[1]);
    return 0;
}

/*
 * Function:
 *    uart_spi_status_clear
 * Purpose:
 *    Routine to clear spi status for Robo2 devices using UART interface.
 * Parameters:
 *    dev - device number
 * Returns:
 *    robo2_spi_status_t
 */
int
uart_spi_status_clear(int dev)
{
    uint8 tx[2];
    int bytes_written = 0;
    int fd = sys_uart_devs[dev].uart_fd;

    /* Before writing read the data to make sure buffer is empty */
    if (uart_read_till_empty(fd, tx, ARRAY_SIZE(tx)) != 0) {
        return -1;
    }
    tx[0] = ARRAY_SIZE(tx);
    tx[1] = ROBO2_SPI_COMMAND(ROBO2_SPI_CLR_SPI_STATUS);
    bytes_written = write(fd, tx, ARRAY_SIZE(tx));
    if (bytes_written != ARRAY_SIZE(tx)) {
        sal_printf("UART write error: bytes written %d\n", bytes_written);
        return -1;
    }
    /* Dummy read */
    if (uart_read(fd, tx, ARRAY_SIZE(tx)) != 0) {
        return -1;
    }
    return 0;
}

/*
 * Function:
 *    _sys_uart_dev_init
 * Purpose:
 *    Initialize to setup linux uart dev driver
 * Parameters:
 *    dev - device number
 *    fd - UART device descriptor
 * Returns:
 *    0 - Success
 *   -1 - Failed
 */
static int
_sys_uart_dev_init(int dev, int fd)
{
    struct termios ser_port_cfg;
    speed_t baud_rate;

    sys_uart_devs[dev].uart_fd = fd;
    sal_memset(&ser_port_cfg, 0, sizeof(ser_port_cfg));
    switch(USER_UART_SPI_BAUD_RATE) {
        case 50:
           baud_rate = B50;
           break;
        case 75:
           baud_rate = B75;
           break;
        case 110:
           baud_rate = B110;
           break;
        case 134:
           baud_rate = B134;
           break;
        case 150:
           baud_rate = B150;
           break;
        case 200:
           baud_rate = B200;
           break;
        case 300:
           baud_rate = B300;
           break;
        case 600:
           baud_rate = B600;
           break;
        case 1200:
           baud_rate = B1200;
           break;
        case 1800:
           baud_rate = B1800;
           break;
        case 4800:
           baud_rate = B4800;
           break;
        case 9600:
           baud_rate = B9600;
           break;
        case 19200:
           baud_rate = B19200;
           break;
        case 38400:
           baud_rate = B38400;
           break;
        case 57600:
           baud_rate = B57600;
           break;
        case 115200:
           baud_rate = B115200;
           break;
        case 230400:
           baud_rate = B230400;
           break;
        default:
           sal_printf("Baud rate not supported %d\n", USER_UART_SPI_BAUD_RATE);
           break;
    }

   /* Get the current attributes of the Serial port */
    tcgetattr(fd, &ser_port_cfg);

    /* Set Read  Speed */
    cfsetispeed(&ser_port_cfg, baud_rate);
    /* Set Write Speed */
    cfsetospeed(&ser_port_cfg, baud_rate);
    /* set up  for non canonical mode */
    cfmakeraw(&ser_port_cfg);
    /* Setting Time outs */
    ser_port_cfg.c_cc[VMIN] = 0;
    /* Wait in deciseconds */
    ser_port_cfg.c_cc[VTIME] = 5;

    /* Set the attributes to the termios structure*/
    if((tcsetattr(fd, TCSANOW, &ser_port_cfg)) != 0)  {
        sal_printf("\n  ERROR ! in Setting attributes");
    }
    return 0;
}
/*
 * Function:
 *    _sys_spi_dev_deinit
 * Purpose:
 *    close linux UART device file
 * Parameters:
 *    dev - device number
 * Returns:
 *    void
 */
static void
_sys_uart_dev_deinit(int dev)
{
    close(sys_uart_devs[dev].uart_fd);
}

/*
 * Function:
 *    _sys_uart_dev_check
 * Purpose:
 *    Check the Chip ID
 * Parameters:
 *    ChipID, RevID
 * Returns:
 *    0 - Found valid SPI devices
 *    1 - No valid SPI devices found
 */
static int
_sys_uart_dev_check(uint32 chipid, uint32 revid)
{
    switch (chipid) {
    case BCM53112_DEVICE_ID:
    case BCM53154_DEVICE_ID:
    case BCM53156_DEVICE_ID:
    case BCM53157_DEVICE_ID:
    case BCM53158_DEVICE_ID:
    case BCM53161_DEVICE_ID:
    case BCM53162_DEVICE_ID:
        return 0;
    }
    /* No valid SPI devices found */
    return 1;
}

/*
 * Function:
 *     linux_uartdev_setup
 * Purpose:
 *     Setup UART access
 * Parameters:
 *     devid	 - device number
 * Returns:
 *     0 - Success
 *    -1 - Failure
 */
static int
linux_uartdev_setup(int devid)
{
    int rv = 0;
    int fd = 0;
    uint32 chipid = 0, revid = 0;

    uart_spi_mutexid = sal_mutex_create("spi access");
    if (uart_spi_mutexid == NULL) {
        sal_printf("UART: Mutex creation failed\n");
        return -1;
    }
    /* Open the device file */
    sal_printf("UART: Open file %s\n", USER_UART_SPI_DEVICE_NAME);
    fd = open(USER_UART_SPI_DEVICE_NAME, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("UART: Open failed\n");
        return -1;
    } else {
        /* Open successful and then initialize to setup the SPI device */
        if (_sys_uart_dev_init(devid, fd) == 0) {
            /* Setup successful. Reset spi slave interface to known state */
            uart_spi_reset(devid, 1);
            sal_sleep(1);

            /* Try to read device id */
            rv = uart_spi_dev_read(devid, BCM53158_A0_CRU_CHIP_ID_REG,
                                   (uint8 *)&chipid, 4);
            if (rv == 0) {
                rv = uart_spi_dev_read(devid, BCM53158_A0_CRU_CHIP_REVID_REG,
                                       (uint8 *)&revid, 4);
            }
            if (rv < 0) {
                 /* Read failed for Linux spidev driver */
                 sal_printf("UART SPI: ChipID read failed\n");
                _sys_uart_dev_deinit(devid);
                return -1;
            } else {
                /* Read successful and check if it is a valid SPI device */
#ifdef BE_HOST
                chipid = sal_htonl(chipid);
                revid = sal_htonl(revid);
#endif
                chipid &= 0xFFFFFF; /* 24 bit chipid */
                revid &= 0xFF; /* 8 bit revid */
                sal_printf("UART SPI: ChipID BCM%d RevID %d\n", chipid, revid);
                rv = _sys_uart_dev_check(chipid, revid);
                if (rv != 0) {
                   /* Not a recognized chip */
                    sal_printf("UART SPI:Unrecognized Chip (chipid %d)\n", chipid);
                   _sys_uart_dev_deinit(devid);
                   return -1;
                }
            }
        }
    }
    return 0;
}


/*
 * External Interfaces
 */

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
int
soc_uart_spi_init(int unit)
{
    int rv = 0;
    if (unit == 0) {
        rv = linux_uartdev_setup(uart_dev[unit]);
    }
    return rv;
}

/*
 * Function:
 *    soc_uart_spi_terminate
 * Purpose:
 *    Close UART wrapper interface
 * Parameters:
 *    unit - unit number
 * Returns:
 *    0 - Success
 *   -1 - Failure
 */
int
soc_uart_spi_terminate(int unit)
{
    if (unit == 0) {
        _sys_uart_dev_deinit(uart_dev[unit]);
    }
    return 0;
}

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
int
soc_uart_spi_read(int unit, uint32 reg, uint8 *buffer, int len)
{
    robo2_spi_status_t ret;

    sal_mutex_take(uart_spi_mutexid, sal_mutex_FOREVER);

    ret = uart_spi_dev_read(uart_dev[unit], reg, buffer, len);
    if (ret) {
        /* Clear the status and try once */
        sal_printf("Clear Status and Redo\n");
        uart_spi_status_clear(uart_dev[unit]);
        ret = uart_spi_dev_read(uart_dev[unit], reg, buffer, len);
    }
    if (ret) {
        sal_printf(" UART SPI: Read Reg(0x%08x) Failed (Reason: %s)\n",
                   reg, uart_spi_errmsg(ret));
        ret = -1;
    }
    sal_mutex_give(uart_spi_mutexid);
    return ret;
}

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
int
soc_uart_spi_write(int unit, uint32 reg, uint8 *buffer, int len)
{
    robo2_spi_status_t ret;

    sal_mutex_take(uart_spi_mutexid, sal_mutex_FOREVER);

    ret = uart_spi_dev_write(uart_dev[unit], reg, buffer, len);
    if (ret) {
        /* Clear the status and try once */
        uart_spi_status_clear(uart_dev[unit]);
        ret = uart_spi_dev_write(uart_dev[unit], reg, buffer, len);
    }
    if (ret) {
        sal_printf("UART SPI: Write to Reg(0x%08x) Failed (Reason: %s)\n",
                   reg, uart_spi_errmsg(ret));
        ret = -1;
    }
    sal_mutex_give(uart_spi_mutexid);
    return ret;
}
