/*
 * $Copyright: (c) 2017 Broadcom Limited
 * All Rights Reserved.$
 *
 * $Id: sal_console.c Exp $
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

 /* size_t is already defined */
#define _SIZE_T
#include "common/system.h"


#if CFG_CONSOLE_ENABLED
    STATIC char lastchar;
#endif /* CFG_CONSOLE_ENABLED */

#if CFG_CONSOLE_ENABLED

size_t sal_strlen(const char *s)
{
    return strlen(s);
}

char *
sal_strchr(const char *dest,int c)
{
    return strchr(dest, c);
}



/*  *********************************************************************
    *  C library platform denpent uart driver
    *
    *  put_char and get_char
    *
    ********************************************************************* */

char put_char(char c) {
    uint32 read_data;
    uint8 unit = 0;

    bcm53158_reg_get(unit, REG_M7SS_UART_UART_LSR, &read_data);
    while ( (read_data & AVR_LSR_TXRDY) == 0) {
        bcm53158_reg_get(unit, REG_M7SS_UART_UART_LSR, &read_data);
    };
    bcm53158_reg_set(unit, REG_M7SS_UART_UART_RBR_THR_DLL, c);
    return c;
}


char get_char(void) {
    char c;
    uint32 read_data;
    uint8 unit = 0;

    bcm53158_reg_get(unit, REG_M7SS_UART_UART_LSR, & read_data);
    while( (read_data & AVR_LSR_RXRDY) == 0) {
        bcm53158_reg_get(unit, REG_M7SS_UART_UART_LSR, & read_data);
    };
    bcm53158_reg_get(unit, REG_M7SS_UART_UART_RBR_THR_DLL, &read_data);
    c = (char )(read_data & 0xFF);
    return c;
}


/*  *********************************************************************
    *  um_console_write()
    *
    *  Console output function
    *
    ********************************************************************* */

static int
um_console_write(const char *buffer,int length)
{
     int blen = length;
    const char *bptr = buffer;
    while (blen > 0) {
        put_char(*bptr);
        bptr++;
        blen--;
    }
    return 0;
}



/*  *********************************************************************
    *  um_console_print()
    *
    *  Console output function
    *
    ********************************************************************* */
int
um_console_print(const char *str)
{
     int count = 0;
     int len;
    char *p;

    /* Convert CR to CRLF as we write things out */
    while ((p = sal_strchr(str,'\n'))) {
        um_console_write(str,p-str);
        um_console_write("\r\n",2);
        count += (p-str);
        str = p + 1;
    }

    len = sal_strlen(str);
    um_console_write(str, len);
    count += len;

    return count;
}

void
sal_console_init(void)
{

}

#endif /* CFG_CONSOLE_ENABLED */

void
APIFUNC(sal_printf)(const char *fmt, ...) REENTRANT
{
#if CFG_CONSOLE_ENABLED
    va_list arg_ptr;
     STATIC char buf[80];

    va_start(arg_ptr, fmt);
    vsprintf(buf, fmt, arg_ptr);
    va_end(arg_ptr);

    um_console_print(buf);
#else
    UNREFERENCED_PARAMETER(fmt);
#endif
}

void
APIFUNC(sal_assert)(const char *expr, const char *file, uint16 line) REENTRANT
{
#if CFG_CONSOLE_ENABLED
    sal_printf("ERROR: Assertion failed: (%s) at %s:%u\n", expr, file, line);
#endif /* CFG_CONSOLE_ENABLED */
    for(;;);
}

char
APIFUNC(sal_getchar)(void) REENTRANT
{
#if CFG_CONSOLE_ENABLED

    lastchar = get_char();
    switch (lastchar) {
        case 0x7f:
        case '\b':
            break;
        case '\r':
        case '\n':
            um_console_write("\r\n",2);
            break;
        default:
            if (lastchar >= ' ') {
                um_console_write(&lastchar,1);
            }
            break;
    }

    return lastchar;
#else
    for(;;) {
        POLL();
    }
    return 0;
#endif
}

char
APIFUNC(sal_get_last_char)(void) REENTRANT
{
#if CFG_CONSOLE_ENABLED
    return lastchar;
#else
    return 0;
#endif
}



char
APIFUNC(sal_putchar)(char c) REENTRANT
{
#if CFG_CONSOLE_ENABLED
    switch (c) {
        case '\b':
            um_console_write("\b \b",3);
        break;
        case '\r':
        case '\n':
            um_console_write("\r\n",2);
        break;
        default:
            um_console_write(&c,1);
        break;
    }
#endif
    return c;
}

