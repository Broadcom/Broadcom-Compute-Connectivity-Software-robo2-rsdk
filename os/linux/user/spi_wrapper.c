/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * Filename:
 *    spi_wrapper.c
 * Description:
 *    robo2 SPI wrapper for user mode access
 */


#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include <linux/types.h>
#include <linux/spi/spidev.h>
#else
#error "SPI Wrapper does not support older than 2.6.27 kernels"
#endif

#include <soc/drv.h>
#include <soc/robo2/bcm53158/init.h>
#include <soc/robo2/bcm53158/spi.h>
#include <osal/sal_console.h>
#include <osal/sal_alloc.h>
#include <osal/sal_util.h>
#include <osal/sal_time.h>
#include <sal_sync.h>

#define MAX_SPI_DEVICES       1
#define MAX_SPI_RETRIES       100

#ifdef BCM_NORTHSTAR_PLUS_SUPPORT
#define USER_SPI_DEVICE_NAME    "/dev/spidev1"
#define USER_SPI_INTF_DEFAULT             "1"
#else
#define USER_SPI_DEVICE_NAME    "/dev/spidev32766"
#define USER_SPI_INTF_DEFAULT             "0"        /*  SPI0 */
#endif

#define USER_SPI_FREQ_DEFAULT        2000000         /*  2MHz */
#define USER_SPI_FREQ_20MHZ         20000000         /* 20MHz */
#define USER_SPI_FREQ_MAX           40000000         /* 40MHz */

#define DEFAULT_SPI_TIMEOUT 50

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
/*
 * Actual spi devices in our system
 */
static struct {
    int    unit;
    int    spi_fd; /* SPI device descriptor */
    uint8  spi_mode;
    uint8  spi_bits;
    uint32 spi_speed;
    uint16 spi_delay;
} _sys_devs[MAX_SPI_DEVICES];

sal_mutex_t spi_mutexid = NULL;
/*
 * Table defines which SPI interface corresponds to the unit
 */
static int spidev[SOC_MAX_NUM_DEVICES] = {
      0,                        /* Unit 0 */
      0                         /* Unit 1, accessed via Unit 0 */
 };

char *spi_status[] = {
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
spi_errmsg(int code)
{
    if ((code >= 0) && (code < sizeof(spi_status)/sizeof(char*))) {
        return spi_status[code];
    } else {
        return "spi transaction failed";
    }
}

/*
 * Function
 *    spi_wait_for_read
 * Purpose
 *   Poll the SPI slave till slave is ready for read data to be delivered
 * Parameters
 *    dev - device number
 *    status - (out) spi status of last read transaction
 * Returns:
 *    0 - Success
 *   -1 - Failed
 */
int
spi_wait_for_read(int dev, uint16 *status)
{
    int ret, i, cnt;
    int fd = _sys_devs[dev].spi_fd;

    uint8  tx[1];
    uint8  rx[4];
    uint16 sts;


    struct spi_ioc_transfer tr[2];

    sal_memset(&rx, 0, ARRAY_SIZE(rx) * sizeof(uint8));

    tx[0] = ROBO2_SPI_COMMAND(ROBO2_SPI_READ_STATUS);

    sal_memset(tr, 0, sizeof(tr));
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = ARRAY_SIZE(tx);
    tr[1].rx_buf = (unsigned long)rx;
    tr[1].len = ARRAY_SIZE(rx);
    cnt = 0;
    do {
        ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
        if (ret == 1) {
            printf("Can't send spi message: read(ret=%d)\n", ret);
            return -1;
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
        if (++cnt > MAX_SPI_RETRIES) { break;}
    } while (sts == ROBO2_SPI_TRANS_STS_PROGRESS);
    spi_status_get(dev, &sts);
    if (status) *status= sts;
    return -1;
}

/*
 * Function
 *    spi_wait_for_write
 * Purpose
 *   Poll the SPI slave till slave has processed all write data
 * Parameters
 *    dev - device number
 *    status - (out) spi status of last write operation
 * Returns:
 *    0 - Success
 *   -1 - Failed
 */
int
spi_wait_for_write(int dev, uint16 *status)
{
    int ret, i, cnt;
    int fd = _sys_devs[dev].spi_fd;

    uint8  tx[1];
    uint8  rx[4];
    uint16 sts;

    struct spi_ioc_transfer tr[2];

    sal_memset(&rx, 0, ARRAY_SIZE(rx) * sizeof(uint8));

    tx[0] = ROBO2_SPI_COMMAND(ROBO2_SPI_GET_WRITE_STS);

    sal_memset(tr, 0, sizeof(tr));
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = ARRAY_SIZE(tx);
    tr[1].rx_buf = (unsigned long)rx;
    tr[1].len = ARRAY_SIZE(rx);
    cnt = 0;
    do {
        ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
        if (ret == 1) {
            printf("Can't send spi message: read(ret=%d)\n", ret);
            return -1;
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
        if (++cnt > MAX_SPI_RETRIES) { break;}
    } while (sts == ROBO2_SPI_TRANS_STS_PROGRESS);
    spi_status_get(dev, &sts);
    if (status) *status= sts;
    return -1;
}

/*
 * Function:
 *    spi_dev_read
 * Purpose:
 *    Routine to send spi read message for Robo2 devices.
 * Parameters:
 *    dev - device number
 *    reg - register addr to read
 *    buf - (out) buffer addr to store the reading result
 *    len - number bytes to read
 * Returns:
 *    robo2_spi_status_t
 */
static robo2_spi_status_t
spi_dev_read(int dev, uint32 reg, uint8 *buf, int len)
{
    robo2_spi_status_t ret;
    int fd = _sys_devs[dev].spi_fd;

    uint8 tx[5];
    uint8 rx[32];
    uint16 status;
    struct spi_ioc_transfer tr[2];

    sal_memset(&rx, 0, ARRAY_SIZE(rx) * sizeof(uint8));

    reg = sal_htonl(reg);
    tx[0] = ROBO2_SPI_COMMAND(ROBO2_SPI_READ_REQ);
    tx[1] = (reg >> 24) & 0xff;
    tx[2] = (reg >> 16) & 0xff;
    tx[3] = (reg >>  8) & 0xff;
    tx[4] = reg & 0xff;

    sal_memset(tr, 0, sizeof(tr));
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = ARRAY_SIZE(tx);
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret == 1) {
        printf("Can't send spi message: read(ret=%d)\n", ret);
        return -1;
    }

    ret = spi_wait_for_read(dev, &status);
    if (ret < 0) {
        printf("spi wait for read failed\n");
        return status;
    }

    sal_memset(tr, 0, sizeof(tr));
    tx[0] = ROBO2_SPI_COMMAND(ROBO2_SPI_READ_DATA);
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = 1;
    tr[1].rx_buf = (unsigned long)buf;
    tr[1].len = len;
    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
    if (ret == 1) {
        printf("Can't send spi message: read(ret=%d)\n", ret);
        return -1;
    }

    return 0;
}

/*
 * Function:
 *    _spi_dev_write
 * Purpose:
 *    Routine to send spi write message for Robo2 devices.
 * Parameters:
 *    dev - device number
 *    reg - register addr to write
 *    buf - (out) buffer addr to store the reading result
 *    len - number bytes to write
 * Returns:
 *    robo2_spi_status_t
 */
static robo2_spi_status_t
spi_dev_write(int dev, uint32 reg, uint8 *buf, int len)
{
    robo2_spi_status_t ret;
    int fd = _sys_devs[dev].spi_fd;
    uint8 tx[32];
    struct spi_ioc_transfer tr[2];
    uint16 status = 0;
    int count;

    /* Fill the address */
    reg = sal_htonl(reg);
    tx[0] = ROBO2_SPI_COMMAND(ROBO2_SPI_WRITE_REQUEST);
    tx[1] = (reg >> 24) & 0xff;
    tx[2] = (reg >> 16) & 0xff;
    tx[3] = (reg >>  8) & 0xff;
    tx[4] = reg & 0xff;

    /* Fill the data */
    for (count = 0; count < len; count++) {
        tx[5 + count] = buf[count];
    }
    sal_memset(tr, 0, sizeof(tr));

    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = 5 + len;
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret == 1) {
        printf("Can't send spi message: read(ret=%d)\n", ret);
        return -1;
    }
    ret = spi_wait_for_write(dev, &status);
    if (ret < 0) {
        printf("spi wait for read failed\n");
        return -1;
    }

    return 0;
}

#ifdef SPI_READ_BURST
/*
 * Function:
 *    _spi_dev_read_burst
 * Purpose:
 *    Routine to send spi read message for Robo2 devices.
 * Parameters:
 *    dev - device number
 *    reglist - list of registers to read upto 8
 *    numregs - num of registers in list
 *    buf - (out) buffer addr to store the reading result
 *    len - number bytes to read
 * Returns:
 *    robo2_spi_status_t
 */
robo2_spi_status_t
spi_dev_read_burst(int dev, uint32 *reglist, int numregs, uint8 *buf, int len)
{
    robo2_spi_status_t ret;
    int i, j;
    uint32 reg;
    int fd = _sys_devs[dev].spi_fd;

    uint8 tx[33];
    uint8 rx[32];
    uint16 status;
    struct spi_ioc_transfer tr[1];

    sal_memset(&rx, 0, ARRAY_SIZE(rx) * sizeof(uint8));

    for (j = 1, i = 0; i < numregs; i++, j+=4) {
        reg = sal_htonl(reglist[i]);
        tx[j + 0] = (reg >> 24) & 0xff;
        tx[j + 1] = (reg >> 16) & 0xff;
        tx[j + 2] = (reg >>  8) & 0xff;
        tx[j + 3] = reg & 0xff;
    }
    i = numregs << 2;
    tx[0] = ROBO2_SPI_BURST_COMMAND(ROBO2_SPI_READ_REQ, i);

    sal_memset(tr, 0, sizeof(tr));
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = ARRAY_SIZE(tx);
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret == 1) {
        printf("Can't send spi message: read(ret=%d)\n", ret);
        return -1;
    }

    ret = spi_wait_for_read(dev, &status);
    if (ret < 0) {
        return -1;
    }

    sal_memset(tr, 0, sizeof(tr));
    tr[0].rx_buf = (unsigned long)buf;
    tr[0].len = len;
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret == 1) {
        printf("Can't send spi message: read(ret=%d)\n", ret);
        return -1;
    }

    return 0;
}
#endif

#ifdef SPI_WRITE_BURST

/*
 * Function:
 *    _spi_dev_write_burst
 * Purpose:
 *    Function to send spi write message for Robo2 devices.
 * Parameters:
 *    dev - device number
 *    reglist - register addr to write
 *    numregs - number of registers
 *    buf - (out) buffer addr to store the reading result
 *    len - number bytes to write
 * Returns:
 *    robo2_spi_status_t
 */
robo2_spi_status_t
spi_dev_write_burst(int dev, uint32* reglist, int numregs, uint8 *buf, int len)
{
    int i, j;
    uint32 reg;
    uint16 status = 0;
    int fd = _sys_devs[dev].spi_fd;

    uint8 tx[5];
    uint8 rx[32];
    struct spi_ioc_transfer tr[2];

    sal_memset(&rx, 0, ARRAY_SIZE(rx) * sizeof(uint8));

    for (j = 1, i = 0; i < numregs; i++, j+=4) {
        reg = sal_htonl(reglist[i]);
        tx[j + 0] = (reg >> 24) & 0xff;
        tx[j + 1] = (reg >> 16) & 0xff;
        tx[j + 2] = (reg >>  8) & 0xff;
        tx[j + 3] = reg & 0xff;
    }
    i = numregs << 2;
    tx[0] = ROBO2_SPI_BURST_COMMAND(ROBO2_SPI_WRITE_REQ, i);

    sal_memset(tr, 0, sizeof(tr));
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = ARRAY_SIZE(tx);
    tr[1].tx_buf = (unsigned long)buf;
    tr[1].len = len;
    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
    if (ret == 1) {
        printf("Can't send spi message: read(ret=%d)\n", ret);
        return -1;
    }
    ret = spi_wait_for_write(dev, &status);
    if (ret < 0) {
        printf("spi wait for read failed\n");
        return -1;
    }

    return 0;
}
#endif /* SPI_WRITE_BURST */

#ifdef USER_SPI_FAST_TRANSACTION
/*
 * Function:
 *    _spi_dev_fast_read
 * Purpose:
 *    Routine to send spi read message for Robo2 devices.
 * Parameters:
 *    dev - device number
 *    reg - register addr to read
 *    buf - (out) buffer addr to store the reading result
 *    len - number bytes to read
 * Returns:
 *    robo2_spi_status_t
 */
static robo2_spi_status_t
spi_dev_fast_read(int dev, uint32 reg, uint8 *buf, int len)
{
    robo2_spi_status_t ret;
    int i, j;
    int fd = _sys_devs[dev].spi_fd;

    uint8 tx[5];
    uint8 rx[32];
    struct spi_ioc_transfer tr[2];

    sal_memset(&rx, 0, ARRAY_SIZE(rx) * sizeof(uint8));

    reg = sal_htonl(reg);
    tx[0] = ROBO2_SPI_COMMAND(ROBO2_SPI_FAST_READ);
    tx[1] = (reg >> 24) & 0xff;
    tx[2] = (reg >> 16) & 0xff;
    tx[3] = (reg >>  8) & 0xff;
    tx[4] = reg & 0xff;

    sal_memset(tr, 0, sizeof(tr));
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = ARRAY_SIZE(tx);

    tr[1].rx_buf = (unsigned long)rx;
    tr[1].len = ARRAY_SIZE(rx);

    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
    if (ret == 1) {
        printf("Can't send spi message: read(ret=%d)\n", ret);
        return -1;
    }
    for (i = 0; i < ARRAY_SIZE(rx); i++) {
        if (rx[i] == 2) {
            for(j=0; j < 4; j++) {
                *(buf + j) = rx[i + j];
            }
            return 0;
        } else if (rx[i] == 3) {
            break;
        }
    }
    return -1;
}
#endif

#ifdef USER_SPI_FAST_TRANSACTION
/*
 * Function:
 *    _spi_dev_fast_write
 * Purpose:
 *    Routine to send spi write message for Robo2 devices.
 * Parameters:
 *    dev - device number
 *    reg - register addr to write
 *    buf - (out) buffer addr to store the reading result
 *    len - number bytes to write
 * Returns:
 *    robo2_spi_status_t
 */
static robo2_spi_status_t
spi_dev_fast_write(int dev, uint32 reg, uint8 *buf, int len)
{
    robo2_spi_status_t ret;
    int i, j;
    int fd = _sys_devs[dev].spi_fd;

    uint8 tx[5];
    uint8 rx[32];
    struct spi_ioc_transfer tr[3];

    sal_memset(&rx, 0, ARRAY_SIZE(rx) * sizeof(uint8));

    reg = sal_htonl(reg);
    tx[0] = ROBO2_SPI_COMMAND(ROBO2_SPI_FAST_WRITE);
    tx[1] = (reg >> 24) & 0xff;
    tx[2] = (reg >> 16) & 0xff;
    tx[3] = (reg >>  8) & 0xff;
    tx[4] = reg & 0xff;

    sal_memset(tr, 0, sizeof(tr));
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = ARRAY_SIZE(tx);
    tr[1].tx_buf = (unsigned long)buf;
    tr[1].len = len;
    tr[2].rx_buf = (unsigned long)rx;
    tr[2].len = ARRAY_SIZE(rx);

    ret = ioctl(fd, SPI_IOC_MESSAGE(3), &tr);
    if (ret == 1) {
        printf("Can't send spi message: read(ret=%d)\n", ret);
        return -1;
    }
    for (i = 0; i < ARRAY_SIZE(rx); i++) {
        if (rx[i] == 1) {
            for(j=0; j < 4; j++) {
                *(buf + j) = rx[i + j];
            }
            return 0;
        } else if (rx[i] == 3) {
            break;
        }
    }
    return -1;
}
#endif

int
spi_reset(int dev, int resetall)
{
    int ret;
    uint8 tx[1];
    struct spi_ioc_transfer tr[2];

    int fd = _sys_devs[dev].spi_fd;

    if (resetall) {
        tx[0] = ROBO2_SPI_COMMAND(ROBO2_SPI_RESET_CHIP);
    } else {
        tx[0] = ROBO2_SPI_COMMAND(ROBO2_SPI_RESET_SPI_BLK);
    }

    sal_memset(tr, 0, sizeof(tr));
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = ARRAY_SIZE(tx);
    /* Dummy read */
    tr[1].rx_buf = (unsigned long)tx;
    tr[1].len = ARRAY_SIZE(tx);

    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
    if (ret == 1) {
        printf("Can't send spi message: read(ret=%d)\n", ret);
        return -1;
    }
    return 0;
}

int
spi_status_get(int dev, uint16 *status)
{
    int ret;
    uint8 tx[1];
    uint8 rx[2];
    struct spi_ioc_transfer tr[2];

    if (!status) {
        return -1;
    }
    int fd = _sys_devs[dev].spi_fd;

    tx[0] = ROBO2_SPI_COMMAND(ROBO2_SPI_GET_SPI_STATUS);

    sal_memset(tr, 0, sizeof(tr));
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = ARRAY_SIZE(tx);
    tr[1].rx_buf = (unsigned long)rx;
    tr[1].len = ARRAY_SIZE(rx);

    ret = ioctl(fd, SPI_IOC_MESSAGE(2), &tr);
    if (ret == 1) {
        printf("Can't send spi message: read(ret=%d)\n", ret);
        return -1;
    }
    *status = ((rx[0] << 8) | rx[1]);
    return 0;
}

int
spi_status_clear(int dev)
{
    int ret;
    uint8 tx[2];
    struct spi_ioc_transfer tr[1];

    int fd = _sys_devs[dev].spi_fd;

    tx[0] = ROBO2_SPI_COMMAND(ROBO2_SPI_CLR_SPI_STATUS);

    sal_memset(tr, 0, sizeof(tr));
    tr[0].tx_buf = (unsigned long)tx;
    /* For dummy read */
    tr[0].rx_buf = (unsigned long)tx;
    tr[0].len = ARRAY_SIZE(tx);

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret == 1) {
        printf("Can't send spi message: read(ret=%d)\n", ret);
        return -1;
    }
    return 0;
}

/*
 * Function:
 *    _sys_spi_dev_init
 * Purpose:
 *    Initialize to setup linux spidev driver
 * Parameters:
 *    dev - device number
 *    fd - SPI device descriptor
 * Returns:
 *    0 - Success
 *   -1 - Failed
 */
static int
_sys_spi_dev_init(int dev, int fd)
{
    uint8 mode;
    uint32 speed;

    _sys_devs[dev].unit = fd;
    _sys_devs[dev].spi_fd = fd;

    mode = ROBO2_SPI_MODE;
    /* Override spi mode from run time env if set */
    if (getenv("ROBO2_SPI_MODE") != NULL) {
        mode = sal_atoi(getenv("ROBO2_SPI_MODE"));
    }

    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
        printf("Can not set mode for SPI device %d.\n", dev);
        close(fd);
        return -1;
    }
    _sys_devs[dev].spi_mode = mode;

    if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0) {
        printf("Can not get max speed for SPI device %d.\n", dev);
        close(fd);
        return -1;
    }

    if (speed != USER_SPI_FREQ_DEFAULT) {
        speed = USER_SPI_FREQ_DEFAULT;
        if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
            printf("Can not set max speed %d Hz for SPI device %d.\n", speed, dev);
            close(fd);
            return -1;
        }
    }
    _sys_devs[dev].spi_speed = speed;

    _sys_devs[dev].spi_bits = 0;
    _sys_devs[dev].spi_delay = 0;

    return 0;
}

/*
 * Function:
 *    _sys_spi_dev_deinit
 * Purpose:
 *    close linux spidev driver device file
 * Parameters:
 *    dev - device number
 * Returns:
 *    void
 */
static void
_sys_spi_dev_deinit(int dev)
{
    int fd;

    fd = _sys_devs[dev].spi_fd;
    close(fd);
}

/*
 * Function:
 *    _sys_spi_dev_check
 * Purpose:
 *    Check the Chip ID
 * Parameters:
 *    ChipID, RevID
 * Returns:
 *    0 - Found valid SPI devices
 *    1 - No valid SPI devices found
 */
static int
_sys_spi_dev_check(int dev, uint32 chipid, uint32 revid)
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
 *     linux_spidev_setup
 * Purpose:
 *     Setup SPI access
 * Parameters:
 *     spidev    - SPI interface to access
 *     spifreq	 - Freq to configure, 0 to select default freq
 * Returns:
 *     0 - Success
 *    -1 - Failure
 */
static int
linux_spidev_setup(char* spidev, uint32 spifreq, int devid)
{
    int rv = 0;
    char spi_device_name[64];
    int fd = 0;
    int spi_freq = 0;
    uint32 chipid = 0, revid = 0;

    sal_snprintf(spi_device_name, ARRAY_SIZE(spi_device_name),
                           "%s.%s", USER_SPI_DEVICE_NAME, spidev);

    if ((spifreq > 0) && (spifreq < USER_SPI_FREQ_MAX)) {
        spi_freq = spifreq;
    } else {
        spi_freq = USER_SPI_FREQ_DEFAULT;
    }

    spi_mutexid = sal_mutex_create("spi access");

    /* Open the device file */
    sal_printf("SPI: Open file %s\n", spi_device_name);
    fd = open(spi_device_name, O_RDWR);
    if (fd < 0) {
        perror("SPI: Open failed\n");
        return -1;
    } else {
        /* Open successful and then initialize to setup the SPI device */
        if (_sys_spi_dev_init(devid, fd) == 0) {
            /* Setup successful. Reset spi slave interface to known state */
            spi_reset(devid, 1);
            sal_sleep(1);
            /* Try to read device phyid low by Linux spidev driver */
            rv = spi_dev_read(devid, BCM53158_A0_CRU_CHIP_ID_REG, (uint8 *)&chipid, 4);
            if (rv == 0) {
                rv = spi_dev_read(devid, BCM53158_A0_CRU_CHIP_REVID_REG, (uint8 *)&revid, 4);
            }
            if (rv < 0) {
                 /* Read failed for Linux spidev driver */
                 sal_printf("SPI: ChipID read failed\n");
                _sys_spi_dev_deinit(devid);
                return -1;
            } else {
                /* Read successful and check if it is a valid SPI device */
#ifdef BE_HOST
                chipid = sal_htonl(chipid);
                revid = sal_htonl(revid);
#endif
                chipid &= 0xFFFFFF; /* 24 bit chipid */
                revid &= 0xFF; /* 8 bit revid */
                sal_printf("SPI: ChipID BCM%d RevID %d\n", chipid, revid);
                rv = _sys_spi_dev_check(devid, chipid, revid);
                if (rv != 0) {
                   /* Not a recognized chip */
                    sal_printf("SPI:Unrecognized Chip (chipid %d)\n", chipid);
                   _sys_spi_dev_deinit(devid);
                   return -1;
                }

                rv = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_freq);
                if (rv < 0) {
                     /* Not a valid SPI device */
                    _sys_spi_dev_deinit(devid);
                    return -1;
                } else {
                    _sys_devs[devid].spi_speed = (uint32)spi_freq;
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
 *    soc_spi_init
 * Purpose:
 *    Initialize SPI wrapper interface
 * Parameters:
 *    unit - unit number
 * Returns:
 *    0 - Success
 *   -1 - Failure
 */
int
soc_spi_init(int unit)
{
    int rv = 0;
    if (unit == 0) {
        rv = linux_spidev_setup(USER_SPI_INTF_DEFAULT, USER_SPI_FREQ_DEFAULT, spidev[unit]);
    }
#ifdef USER_SPI_FAST_TRANSACTION
    printf("SPI: Fast Transaction Mode\n");
#endif
    return rv;
}

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
int
soc_spi_terminate(int unit)
{
    if (unit == 0) {
        _sys_spi_dev_deinit(spidev[unit]);
    }
    return 0;
}

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
int
soc_spi_read(int unit, uint32 reg, uint8 *buffer, int len)
{
    robo2_spi_status_t ret;

    sal_mutex_take(spi_mutexid, sal_mutex_FOREVER);
#ifdef USER_SPI_FAST_TRANSACTION
    ret = spi_dev_fast_read(spidev[unit], reg, buffer, len);
#else
    ret = spi_dev_read(spidev[unit], reg, buffer, len);
    if (ret) {
        /* Clear the status and try once */
        printf("Clear Status and Redo\n");
        spi_status_clear(spidev[unit]);
        ret = spi_dev_read(spidev[unit], reg, buffer, len);
    }
#endif
    if (ret) {
        printf("SPI: Read Reg(0x%08x) Failed (Reason: %s)\n",
               reg, spi_errmsg(ret));
        ret = -1;
    }
    sal_mutex_give(spi_mutexid);

    return ret;
}

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
int
soc_spi_write(int unit, uint32 reg, uint8 *buffer, int len)
{
    robo2_spi_status_t ret;

    sal_mutex_take(spi_mutexid, sal_mutex_FOREVER);
#ifdef USER_SPI_FAST_TRANSACTION
    ret = spi_dev_fast_write(spidev[unit], reg, buffer, len);
#else
    ret = spi_dev_write(spidev[unit], reg, buffer, len);
    if (ret) {
        /* Clear the status and try once */
        spi_status_clear(spidev[unit]);
        ret = spi_dev_write(spidev[unit], reg, buffer, len);
    }
#endif
    if (ret) {
        printf("SPI: Write to Reg(0x%08x) Failed (Reason: %s)\n",
               reg, spi_errmsg(ret));
        ret = -1;
    }
    sal_mutex_give(spi_mutexid);

    return ret;
}
