/*
 * $Id: $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *    uart.h
 * Description:
 *    Uart Defintions
 */


#ifndef __BCM53158_A0_UART_H
#define __BCM53158_A0_UART_H

#include <soc/robo2/bcm53158/config.h>

/* UART defines */
#define UART_CLOCK  (BCM53158_A0_UART_CLOCK)

#ifdef EMULATION_BUILD
#define UART_DEFAULT_BAUDRATE  (9600)
#else
#define UART_DEFAULT_BAUDRATE  (9600)
#endif

#define UART_POLL_FOREVER  (-1)

/* Line Control */
/* Parity Bits */
#define UART_LCR_PARITY_NONE  0x0
#define UART_LCR_PARITY_ODD   0x1
#define UART_LCR_PARITY_EVEN  0x2

/* Data Bits */
#define UART_LCR_DATABITS_5   0x0
#define UART_LCR_DATABITS_6   0x1
#define UART_LCR_DATABITS_7   0x2
#define UART_LCR_DATABITS_8   0x3

/* Stop Bits */
#define UART_LCR_STOPBITS_1     0x0
#define UART_LCR_STOPBITS_1P5   0x1
#define UART_LCR_STOPBITS_2     0x2

/* User Status */
#define UART_USR_BUSY                0x1
#define UART_USR_TX_FIFO_NOT_FULL    0x2
#define UART_USR_TX_FIFO_EMPTY       0x4
#define UART_USR_RX_FIFO_NOT_EMPTY   0x8
#define UART_USR_RX_FIFO_FULL        0x10

/* Line Status */
#define UART_LSR_DATA_READY      0x1
#define UART_LSR_RX_OVERRUN      0x2
#define UART_LSR_PARITY_ERR      0x4
#define UART_LSR_FRAMING_ERR     0x8
#define UART_LSR_BREAK_INTR      0x10
#define UART_LSR_THR_EMPTY       0x20
#define UART_LSR_TX_EMPTY        0x40
#define UART_LSR_RX_FIFO_ERR     0x80

/* Modem Control */
#define UART_MCR_AUTO_FLOWCONTROL 0x20
#define UART_MCR_RTS              0x02
#define UART_MCR_DTR              0x01

/* Flow control */
#define UART_FCR_RX_TRIG_FIFO_FULL          0xC0   /* 2 less than full */
#define UART_FCR_RX_TRIG_FIFO_HALF_FULL     0x80   /* 1/2 full */
#define UART_FCR_RX_TRIG_FIFO_QUARTER_FULL  0x40   /* 1/4 full */
#define UART_FCR_RX_TRIG_FIFO_NOT_EMPTY     0x00   /* trigger on single char */
#define UART_FCR_TX_TRIG_FIFO_HALF_FULL     0x30   /* 1/2 full */
#define UART_FCR_TX_TRIG_FIFO_QUARTER_FULL  0x20   /* 1/4 full */
#define UART_FCR_TX_TRIG_FIFO_NOT_EMPTY     0x10   /* Trigger on 2 chars */
#define UART_FCR_TX_TRIG_FIFO_EMPTY         0x00   /* trigger when empty */
#define UART_FCR_TX_FIFO_RESET              0x03   /* Reset tx fifo */
#define UART_FCR_RX_FIFO_RESET              0x02   /* Reset rx fifo */
#define UART_FCR_FIFO_ENABLE                0x01   /* Enable Fifos */


/* Poll conditions */
#define UART_STS_NOT_BUSY       0x1     /* Uart is not busy */
#define UART_STS_TX_EMPTY       0x2     /* Tx Fifo / THR is empty */
#define UART_STS_RX_AVAILABLE   0x3     /* Rx Fifo is not empty, some chars have arrived */

/* Reg Mapping */
#define UART_BASE         BCM53158_A0_UART_BASE_ADDR
#define UART_RBR_THR_DLL  (UART_BASE + 0x0)
#define UART_DLH_IER      (UART_BASE + 0x4)
#define UART_IIR_FCR      (UART_BASE + 0x8)
#define UART_LCR          (UART_BASE + 0xc)
#define UART_MCR          (UART_BASE + 0x10)
#define UART_LSR          (UART_BASE + 0x14)
#define UART_SRBR_STHR    (UART_BASE + 0x30)
#define UART_USR          (UART_BASE + 0x7c)


int uart_kbhit(void);
int uart_getchar_nb(void);
uint8 uart_getchar(void);
int uart_putchar(char data);
int init_uart(uint32 baudrate);


#endif /* BCM53158_A0_UART_H */
