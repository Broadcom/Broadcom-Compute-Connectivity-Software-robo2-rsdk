/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * File:
 *    robo_uart.h
 * Description:
 *    Uart Defintions
 */


#ifndef __BCM53158_A0_UART_FIFO_H
#define __BCM53158_A0_UART_FIFO_H

#define ROBO_UART_FIFO_LENGTH      64

typedef enum uart_parameter_s {
    UART_DATA_BITS,
    UART_STOP_BITS,
    UART_BAUD_RATE,
    UART_PARITY,
    UART_MODEM_CONTROL,
    UART_HALT_TX
} uart_param_t;

int robo_uart_init(uint32 unit);
int robo_uart_param_set(uint32_t unit, uart_param_t param, uint32_t val);
int robo_uart_tx(uint32_t unit, char *data, uint32_t length);
int robo_uart_rx(uint32_t unit, char *data, uint32_t length);
#endif /* BCM53158_A0_UART_FIFO_H */
