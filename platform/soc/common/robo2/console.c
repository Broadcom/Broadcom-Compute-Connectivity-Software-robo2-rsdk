/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * Filename:
 *    console.c
 * Description:
 *    Console Functionality
 *    
 */


#include <sal_types.h>
#include "sal_sync.h"
#include <soc/types.h>
#include <soc/register.h>
#include <soc/error.h>
#include <soc/robo2/common/allenum.h>
#include <soc/robo2/common/regacc.h>
#include <soc/robo2/common/memregs.h>
#ifdef BCM_53158_A0
#include <soc/robo2/bcm53158/uart.h>
#include <soc/robo2/bcm53158/init.h>
#endif
#include <soc/robo2/common/console.h>

#define CONSOLE_BUFFER_MAX 1024
#define CONSOLE_BUFFER_STASH(ch)                                        \
          do {                                                          \
              *console_pointer++ = ch;                                  \
              if (console_pointer >= console_buffer_end) {              \
                  console_pointer =  &console_buffer[0];                \
              }                                                         \
              console_buffer_cnt ++;                                    \
          } while (0)

static uint8 console_buffer[CONSOLE_BUFFER_MAX];
static uint8* console_pointer = &console_buffer[0];
static uint8* console_buffer_end = &console_buffer[CONSOLE_BUFFER_MAX];
static int console_enabled = 0;
static int console_buffer_cnt = 0;
static sal_mutex_t console_mutex = NULL;

/** @brief Open Conole
 *  Function obtains control of UART exclusively and returns a uart handle
 */
__attribute__((section("fastcode")))
int console_open(uint32 baudrate)
{
    uint8 *p;

    if (console_enabled == 0) {
        init_uart(baudrate);
        console_enabled = 1;
        if (console_mutex == NULL)  {
            console_mutex = sal_mutex_create("console");
        }
        if (console_buffer_cnt > 0) {
            console_puts("Buffered Logs:\n");
            if (console_buffer_cnt > CONSOLE_BUFFER_MAX) {
                for (p = console_pointer; p < console_buffer_end; p++)
                console_putc(*p);
            }
            for (p = &console_buffer[0]; p < console_pointer; p++)
                console_putc(*p);
            console_putc('\n');
            console_putc('\n');
        }
        return 0;       
    }
    return -1;
}

/**
 * @brief Console close
 * This stops console activity
 * Used when console is open in block io mode
 */
__attribute__((section("fastcode")))
int console_close()
{
    console_enabled = 0;
    console_buffer_cnt = 0;
    console_pointer = &console_buffer[0];
    return 0;
}

/**
 * @brief Console Print char
 * Function send a byte out
 */
__attribute__((section("fastcode")))
int console_putc(const char ch)
{
    if (!console_enabled) {
        CONSOLE_BUFFER_STASH(ch);
        return 1;
    }
    if (ch == '\n') {
        uart_putchar('\r');
    }
    uart_putchar(ch);
    return 1;
}

/**
 * @brief Console Print
 * Function send a stream of bytes out
 */
__attribute__((section("fastcode")))
int console_puts(const char *buffer)
{
    int len = 0;
    int ch = 0;

    if (console_mutex)
        sal_mutex_take(console_mutex, 0x7FFFFFFF);

    while((ch = *buffer++)) {
        if (ch == '\n') {
            if (console_enabled) {
                uart_putchar('\r');
            }
        }
        if (console_enabled) {
            uart_putchar(ch);
        } else {
            CONSOLE_BUFFER_STASH(ch);
        }
        len++;
    }

    if (console_mutex)
        sal_mutex_give(console_mutex);

    return len;
}

/**
 * @brief Console getc
 * Function receive a byte
 */
__attribute__((section("fastcode")))
int console_getc(char *ch)
{
    if (!console_enabled)
        return 0;
    if (ch)
        *ch = uart_getchar();
    return 1;
}
 

/**
 * @brief Console backspace
 * Function print count number of backspaces
 */
__attribute__((section("fastcode")))
inline void console_backspace(int count)
{
    while (count--) {
        console_putc('\x08');
        console_putc('\x20');
        console_putc('\x08');
    }
}

/**
 * @brief Console newline
 * Function print a newline
 */
inline void console_newline(void)
{
    console_putc('\r');
    console_putc('\n');
}

/**
 * @brief Console kbhit
 * Returns true if char is waiting
 */
inline int console_kbhit(void)
{
    if (!console_enabled) {
        return 0;
    } else {
        return (uart_kbhit());
    }
}

