/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * Filename:
 *    robo_uart.c
 * Description:
 *    robo UART Driver
 */

#include <sal_types.h>
#include <sal_console.h>
#include <sal_time.h>
#include <sal_sync.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <soc/cpu_m7.h>
#include <soc/robo2/common/allenum.h>
#include <soc/robo2/common/regacc.h>
#include <soc/robo2/bcm53158/uart.h>
#include <soc/robo2/bcm53158/robo_uart.h>
#include <soc/robo2/bcm53158/init.h>
#include <fsal_int/types.h>


sal_mutex_t uart_mutexid[SOC_MAX_NUM_DEVICES];

#define UART_MUTEX_TAKE(unit)      do {                       \
     if (uart_mutexid[unit] == 0) {                           \
         return SOC_E_INTERNAL;                               \
     }                                                        \
     sal_mutex_take(uart_mutexid[unit], sal_mutex_FOREVER);   \
} while(0)

#define UART_MUTEX_GIVE(unit)      do {    \
     if (uart_mutexid[unit] == 0) {        \
         return SOC_E_INTERNAL;            \
     }                                     \
     sal_mutex_give(uart_mutexid[unit]);   \
} while(0)

#define UART_WAIT_IF_BUSY(unit)     do {          \
     int _rval = 0;                               \
     REG_READ_M7SS_UART_UART_USRr(unit, &_rval);  \
     if ((_rval & UART_USR_BUSY) == 0) {break;}   \
} while (1);

/*
 * Function:
 *     robo_uart_param_set
 * Description:
 *     set the uart parameter
 * Parameters:
 *     unit - unit number
 *     param - parameter to set
 *     val - value to set
 * Returns:
 *     SOC_E_XXXX
 */
int robo_uart_param_set(uint32_t unit, uart_param_t param, uint32_t val)
{
    uint32_t lcrreg = 0, divhi = 0, divlo = 0;
    uint32_t divisor = 0;
    uint32_t stop = 0;
    uint32_t tmpval = 0;

    UART_MUTEX_TAKE(unit);
    UART_WAIT_IF_BUSY(unit);
    switch (param) {
        case UART_DATA_BITS:
            if ((val < UART_LCR_DATABITS_5) || (val > UART_LCR_DATABITS_8)) {
                UART_MUTEX_GIVE(unit);
                return SOC_E_PARAM;
            }
            REG_READ_M7SS_UART_UART_LCRr(unit, &lcrreg);
            soc_M7SS_UART_UART_LCRr_field_set(unit, &lcrreg, DLSf, &val);
            REG_WRITE_M7SS_UART_UART_LCRr(unit, &lcrreg);
            break;

        case UART_STOP_BITS:
            stop = (val != UART_LCR_STOPBITS_1) ? 1 : 0;
            REG_READ_M7SS_UART_UART_LCRr(unit, &lcrreg);
            soc_M7SS_UART_UART_LCRr_field_set(unit, &lcrreg, STOPf, &stop);
            REG_WRITE_M7SS_UART_UART_LCRr(unit, &lcrreg);
            break;

        case UART_BAUD_RATE:
            divisor = UART_CLOCK / (16 * val);
            divhi = (divisor >> 8) & 0xFF;
            divlo = divisor & 0xFF;

            tmpval = 1;
            REG_READ_M7SS_UART_UART_LCRr(unit, &lcrreg);
            soc_M7SS_UART_UART_LCRr_field_set(unit, &lcrreg, DLABf, &tmpval);
            REG_WRITE_M7SS_UART_UART_LCRr(unit, &lcrreg);

            divisor = 0;
            soc_M7SS_UART_UART_RBR_THR_DLLr_field_set(unit, &divisor, RBR_THR_DLLf, &divlo);
            REG_WRITE_M7SS_UART_UART_RBR_THR_DLLr(unit, &divisor);

            divisor = 0;
            soc_M7SS_UART_UART_DLH_IERr_field_set(unit, &divisor, DLH_IERf, &divhi);
            REG_WRITE_M7SS_UART_UART_DLH_IERr(unit, &divisor);
            /* Disable DLAB */
            tmpval = 0;
            soc_M7SS_UART_UART_LCRr_field_set(unit, &lcrreg, DLABf, &tmpval);
            REG_WRITE_M7SS_UART_UART_LCRr(unit, &lcrreg);
            break;

        case UART_PARITY:
            REG_READ_M7SS_UART_UART_LCRr(unit, &lcrreg);
            if (val == UART_LCR_PARITY_NONE) {
                tmpval = 0; /* PEN = 0 */
                soc_M7SS_UART_UART_LCRr_field_set(unit, &lcrreg, PENf, &tmpval);
            } else if (val == UART_LCR_PARITY_ODD) {
                tmpval = 1; /* PEN = 1 */
                soc_M7SS_UART_UART_LCRr_field_set(unit, &lcrreg, PENf, &tmpval);
                tmpval = 0; /* EPS = 0 */
                soc_M7SS_UART_UART_LCRr_field_set(unit, &lcrreg, EPSf, &tmpval);
            } else {
                tmpval = 1; /* PEN = 1 */
                soc_M7SS_UART_UART_LCRr_field_set(unit, &lcrreg, PENf, &tmpval);
                tmpval = 1; /* EPS = 1 */
                soc_M7SS_UART_UART_LCRr_field_set(unit, &lcrreg, EPSf, &tmpval);
            }
            REG_WRITE_M7SS_UART_UART_LCRr(unit, &lcrreg);
            break;

        case UART_MODEM_CONTROL:
            REG_WRITE_M7SS_UART_UART_MCRr(unit, &val);
            break;

        case UART_HALT_TX:
            REG_WRITE_M7SS_UART_UART_HTXr(unit, &val);
            break;

        default:
            break;
    }
    UART_MUTEX_GIVE(unit);

    return SOC_E_NONE;
}

/*
 * Function:
 *     robo_uart_init
 * Description:
 *     init the uart
 * Parameters:
 *     unit - unit number
 * Returns:
 *     SOC_E_XXXX
 */
int robo_uart_init(uint32_t unit)
{
    uint32_t regval = 0;

    if (unit == CBX_AVENGER_PRIMARY) {
       uart_mutexid[unit] = sal_mutex_create("pri_uart");
    } else {
       uart_mutexid[unit] = sal_mutex_create("sec_uart");
    }

    /* Wait if UART is busy */
    UART_WAIT_IF_BUSY(unit);

    /* Baud, 8N1 */
    regval = UART_FCR_RX_FIFO_RESET | UART_FCR_TX_FIFO_RESET |
             UART_FCR_FIFO_ENABLE;
    REG_WRITE_M7SS_UART_UART_IIR_FCRr(unit, &regval);

    robo_uart_param_set(unit, UART_DATA_BITS, UART_LCR_DATABITS_8);
    robo_uart_param_set(unit, UART_PARITY, UART_LCR_PARITY_NONE);
    robo_uart_param_set(unit, UART_STOP_BITS, UART_LCR_STOPBITS_1);
    robo_uart_param_set(unit, UART_BAUD_RATE, UART_DEFAULT_BAUDRATE);

    return SOC_E_NONE;
}

/*
 * Function:
 *     robo_uart_is_rx_ready
 * Description:
 *     check if uart rx data is ready
 * Parameters:
 *     unit - unit number
 * Returns:
 *     SOC_E_XXXX
 */
static inline
int robo_uart_is_rx_ready(uint32_t unit)
{
    uint32_t regval = 0;

    REG_READ_M7SS_UART_UART_LSRr(unit, &regval);
    return (regval & UART_LSR_DATA_READY);
}

/*
 * Function:
 *     robo_uart_is_tx_fifo_full
 * Description:
 *     check if uart tx fifo is full
 * Parameters:
 *     unit - unit number
 * Returns:
 *     SOC_E_XXXX
 */
static inline int robo_uart_is_tx_fifo_full(uint32_t unit)
{
    uint32_t regval = 0;

    REG_READ_M7SS_UART_UART_USRr(unit, &regval);
    if (regval & UART_USR_TX_FIFO_NOT_FULL) {
        return 0;
    }
    return 1;
}

/*
 * Function:
 *     robo_uart_tx
 * Description:
 *     transmit data
 * Parameters:
 *     unit - unit number
 *     data - data pointer
 *     length - length to transmit
 * Returns:
 *     SOC_E_XXXX
 *     Errors indicated in negative numbers. The return value
 *     indicates the no of bytes transmitted.
 *
 */
int robo_uart_tx(uint32_t unit, char *data, uint32_t length)
{
    uint32_t bytes, count = 0;
    //uint32_t level;
    uint32_t fifo_cnt;
    uint32_t regval = 0;
    if ((length == 0) || (data == NULL)) {
        sal_printf("Uart Tx Error %d %p\n", length, data);
        return SOC_E_PARAM;
    }

    UART_MUTEX_TAKE(unit);
    while (length - count){
        if ((length - count) > ROBO_UART_FIFO_LENGTH) {
            fifo_cnt = ROBO_UART_FIFO_LENGTH;
        } else {
            fifo_cnt = length - count;
        }

        for (bytes = 0; bytes < fifo_cnt; bytes++) {
            /* if fifo is full, return count */
            if (robo_uart_is_tx_fifo_full(unit)) {
                sal_usleep(3000);
                if (robo_uart_is_tx_fifo_full(unit)) {
                    UART_MUTEX_GIVE(unit);
                    return (count + bytes);
                }
            }
            regval = data[count + bytes];
            REG_WRITE_M7SS_UART_UART_RBR_THR_DLLr(unit, &regval);
        }
        count = count + fifo_cnt;
    }
    UART_MUTEX_GIVE(unit);

    return count;
}

/*
 * Function:
 *     robo_uart_rx
 * Description:
 *     recieve data
 * Parameters:
 *     unit - unit number
 *     data - data pointer
 *     length - length to recieve
 * Returns:
 *     SOC_E_XXXX
 *     Errors indicated in negative numbers. If the fifo has
 *     less data than the provided length, it will return the
 *     number of read bytes from fifo.
 */
int robo_uart_rx(uint32_t unit,char *data, uint32_t length)
{
    uint32_t regval = 0;
    uint32_t byte_cnt = 0;
    uint32_t fifolvl = 0, lsr = 0;
    int i;

    if ((length == 0) || (data == NULL)) {
        return SOC_E_PARAM;
    }

    UART_MUTEX_TAKE(unit);
    REG_READ_M7SS_UART_UART_LSRr(unit, &lsr);
    while (robo_uart_is_rx_ready(unit)) {
        REG_READ_M7SS_UART_UART_RFLr(unit, &fifolvl);
        for (i=0; i < fifolvl; i++) {
            REG_READ_M7SS_UART_UART_RBR_THR_DLLr(unit, &regval);
            data[byte_cnt] = regval & 0xff;
            byte_cnt = byte_cnt + 1;
            if (byte_cnt == length) {
                goto READ_DONE;
            }
        }
    }
    READ_DONE:
    UART_MUTEX_GIVE(unit);
    return byte_cnt;
}

