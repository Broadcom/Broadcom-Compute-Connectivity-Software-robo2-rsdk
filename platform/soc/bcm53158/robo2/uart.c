/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * Filename:
 *    uart.c
 * Description:
 *    UART Driver
 *    There is only one uart to handle
 */


#include <sal_types.h>
#include <sal_console.h>
#include <sal_time.h>
#include <soc/types.h>
#include <soc/drv.h>

#ifdef CORTEX_M7
#include <soc/cpu_m7.h>
#include <soc/robo2/common/allenum.h>
#include <soc/robo2/common/regacc.h>
#include <soc/robo2/bcm53158/uart.h>
#include <soc/robo2/bcm53158/init.h>

uart_rx_callback_fnt uart_rx_handler;
void *uart_rx_handler_context;

__attribute__((section("text.fastcode")))
void uart_handler(void)
{
    uint32 regval = 0;
    uint32 unit = 0;
    char ch;
    m7_disable_intr();
    REG_READ_M7SS_UART_UART_IIR_FCRr(unit, &regval);
    if ((regval & 0xf) == 0x4) {
        REG_READ_M7SS_UART_UART_RBR_THR_DLLr(unit, &regval);
        ch = regval & 0xff;
        if (uart_rx_handler)
            uart_rx_handler(uart_rx_handler_context, ch);
    } else if ((regval & 0xf) == 0x7) {
        REG_READ_M7SS_UART_UART_USRr(unit, &regval);
    }
    m7_enable_intr();
}

void uart_rx_register(uart_rx_callback_fnt userfn, void *context)
{
    uint32 regval = 0;
    uint32 unit = 0;
    uart_rx_handler = userfn;
    uart_rx_handler_context = context;
    REG_READ_M7SS_UART_UART_DLH_IERr(unit, &regval);
    regval |= (UART_IER_RX_DATA);
    REG_WRITE_M7SS_UART_UART_DLH_IERr(unit, &regval);
    m7_enable_irq(UART_IRQn);
}

void uart_rx_unregister(void)
{
    uint32 regval = 0;
    uint32 unit = 0;
    m7_disable_irq(UART_IRQn);
    REG_READ_M7SS_UART_UART_DLH_IERr(unit, &regval);
    regval &= ~(UART_IER_RX_DATA);
    REG_WRITE_M7SS_UART_UART_DLH_IERr(unit, &regval);
    uart_rx_handler = NULL;
}

/**
 * @brief UART Poll
 * Function to poll uart and wait till required conditions are met
 */
static void
uart_poll_until(int timeout, uint32 condition)
{
    uint32 regval;
    if (timeout == UART_POLL_FOREVER) {
        do {
            regval = 0;
            REG_READ_M7SS_UART_UART_USRr(0, &regval);
            switch (condition) {
            case UART_STS_NOT_BUSY:
                if ((regval & UART_USR_BUSY) == 0) return;
                break;
            case UART_STS_TX_EMPTY:
                if (regval & UART_USR_TX_FIFO_EMPTY) return;
                break;
            case UART_STS_RX_AVAILABLE:
                if (regval & UART_USR_RX_FIFO_NOT_EMPTY) return;
                break;
            }
        } while (1);
    } else {
        do {
            regval = 0;
            REG_READ_M7SS_UART_UART_USRr(0, &regval);
            switch (condition) {
            case UART_STS_NOT_BUSY:
                if ((regval & UART_USR_BUSY) == 0) return;
                break;
            case UART_STS_TX_EMPTY:
                if (regval & UART_USR_TX_FIFO_EMPTY) return;
                break;
            case UART_STS_RX_AVAILABLE:
                if (regval & UART_USR_RX_FIFO_NOT_EMPTY) return;
                break;
            }
            sal_usleep(1000);
        } while (--timeout > 0);
    }
}

/**
 * @brief Init UART Flow Control Settings
 * Function initialize the uart Flow control and Fifos
 */
static int
init_uart_fc_settings(uint32 enable, uint32 tx_trig_lvl, uint32 rx_trig_lvl)
{
    uint32 regval = 0;
    if ((rx_trig_lvl > 3) || (tx_trig_lvl > 3)) {
        return SOC_E_PARAM;
    }

    regval = UART_FCR_RX_FIFO_RESET | UART_FCR_TX_FIFO_RESET;
    if (enable) {
        regval |= UART_FCR_FIFO_ENABLE;
        regval |= (rx_trig_lvl | tx_trig_lvl);
    }
    REG_WRITE_M7SS_UART_UART_IIR_FCRr(0, &regval);
    return SOC_E_NONE;

}

/**
 * @brief Init UART Settings
 * Function initialize the uart to desired baud rate, Data, Stop bits, Odd/Even parity
 */
static int
init_uart_settings(uint32 baudrate, uint8 databits, uint8 stopbits, uint8 parity)
{
    uint32 lcrreg = 0, divhi = 0, divlo = 0;
    uint32 divisor = 0;
    uint32 stop    = 0;
    uint32 data    = 0;
    uint32 tmpval  = 0;
    uint32 regval  = 0;
    int delay = 10000;

    if ((databits < UART_LCR_DATABITS_5) || (databits > UART_LCR_DATABITS_8)) {
        return SOC_E_PARAM;
    }
    if ((stopbits == UART_LCR_STOPBITS_1P5) && (databits != UART_LCR_DATABITS_5)) {
        return SOC_E_PARAM;
    }

    stop = (stopbits != UART_LCR_STOPBITS_1) ? 1 : 0;
    data = databits;

    divisor = UART_CLOCK / (16 * baudrate);


    divhi = (divisor >> 8) & 0xFF;
    divlo = divisor & 0xFF;

    /* Wait if the uart is busy */
    m7_disable_intr();

    /* Reset Fifos */
    init_uart_fc_settings(0, 0, 0);

    uart_poll_until(UART_POLL_TIMEOUT, UART_STS_NOT_BUSY);

    soc_M7SS_UART_UART_LCRr_field_set(0, &lcrreg, DLSf, &data);
    soc_M7SS_UART_UART_LCRr_field_set(0, &lcrreg, STOPf, &stop);
    if (parity == UART_LCR_PARITY_NONE) {
        tmpval = 0; /* PEN = 0 */
        soc_M7SS_UART_UART_LCRr_field_set(0, &lcrreg, PENf, &tmpval);
    } else if (parity == UART_LCR_PARITY_ODD) {
        tmpval = 1; /* PEN = 1 */
        soc_M7SS_UART_UART_LCRr_field_set(0, &lcrreg, PENf, &tmpval);
        tmpval = 0; /* EPS = 0 */
        soc_M7SS_UART_UART_LCRr_field_set(0, &lcrreg, EPSf, &tmpval);
    } else {
        tmpval = 1; /* PEN = 1 */
        soc_M7SS_UART_UART_LCRr_field_set(0, &lcrreg, PENf, &tmpval);
        tmpval = 1; /* EPS = 1 */
        soc_M7SS_UART_UART_LCRr_field_set(0, &lcrreg, EPSf, &tmpval);
    }

    /* Enable DLAB and set divisor */
    tmpval = 1;
    soc_M7SS_UART_UART_LCRr_field_set(0, &lcrreg, DLABf, &tmpval);
    REG_WRITE_M7SS_UART_UART_LCRr(0, &lcrreg);
    divisor = 0;
    soc_M7SS_UART_UART_RBR_THR_DLLr_field_set(0, &divisor, RBR_THR_DLLf, &divlo);
    REG_WRITE_M7SS_UART_UART_RBR_THR_DLLr(0, &divisor);

    divisor = 0;
    soc_M7SS_UART_UART_DLH_IERr_field_set(0, &divisor, DLH_IERf, &divhi);
    REG_WRITE_M7SS_UART_UART_DLH_IERr(0, &divisor);

    /* Disable DLAB */
    tmpval = 0;
    soc_M7SS_UART_UART_LCRr_field_set(0, &lcrreg, DLABf, &tmpval);
    REG_WRITE_M7SS_UART_UART_LCRr(0, &lcrreg);

    while(delay--);
    /* Set Modem Control */
    tmpval = UART_MCR_AUTO_FLOWCONTROL | UART_MCR_RTS | UART_MCR_DTR;
    soc_M7SS_UART_UART_LCRr_field_set(0, &regval, DLABf, &tmpval);
    REG_WRITE_M7SS_UART_UART_MCRr(0, &regval);

    /* Enable Fifos */
    init_uart_fc_settings(1, 0, 0);

    m7_enable_intr();

    return SOC_E_NONE;
}

/**
 * @brief Init UART
 * Function initialize the uart to desired baud rate, 8 databits, 1 stop bit, No parity
 */
int init_uart(uint32 baudrate)
{
    /* Baud, 8N1 */
    return init_uart_settings(baudrate, UART_LCR_DATABITS_8,
                              UART_LCR_STOPBITS_1, UART_LCR_PARITY_NONE);
}

/**
 * @brief UART Is Rx Ready
 * Function returns true if Rx data is available
 */
static inline
int uart_is_rx_ready(void)
{
    uint32 regval = 0;
    REG_READ_M7SS_UART_UART_LSRr(0, &regval);
    return (regval & UART_LSR_DATA_READY);
}

/**
 * @brief UART Is Tx Done
 * Function returns true if Tx is done
 */
static inline
int uart_is_tx_done(void)
{
    uint32 regval = 0;
    REG_READ_M7SS_UART_UART_LSRr(0, &regval);
    return ((regval & UART_LSR_THR_EMPTY) != 0);
}

/**
 * @brief UART Putchar
 * Function Send a char out
 */
__attribute__((section("fastcode")))
int uart_putchar(char data)
{
    while(!uart_is_tx_done());
    uint32 regval = data;
    REG_WRITE_M7SS_UART_UART_RBR_THR_DLLr(0, &regval);
    return SOC_E_NONE;
}

/**
 * @brief UART Getchar
 * Function Wait for a char and get it
 */
__attribute__((section("fastcode")))
uint8 uart_getchar(void)
{
    uint32 regval = 0;
    while(!uart_is_rx_ready());
    REG_READ_M7SS_UART_UART_RBR_THR_DLLr(0, &regval);
    return (regval & 0xff);
}

/**
 * @brief UART Getchar Non Blocking
 * Function Get char if available or return -1
 */
int uart_getchar_nb(void)
{
    uint32 regval = 0;
    if (uart_is_rx_ready()) {
        REG_READ_M7SS_UART_UART_RBR_THR_DLLr(0, &regval);
        return (regval & 0xff);
    } else {
        return -1;
    }
}

/**
 * @brief uart kbhit
 * routine returns true if kbhit, false otherwise
 */
inline
int uart_kbhit(void)
{
    return(uart_is_rx_ready());
}


#else /* CORTEX_M7 */

int
init_uart(uint32 baudrate)
{
    return -1;
}

int
uart_putchar(char data)
{
    return -1;
}

uint8
uart_getchar(void)
{
    return 0;
}

int
uart_getchar_nb(void)
{
    return -1;
}

inline int
uart_kbhit(void)
{
    return -1;
}

#endif /* CORTEX_M7 */
