/* 
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 * 
 * File:
 *    system.c
 * Description:
 *    Implementation for bcm53158 system routines
 */

#ifdef CORTEX_M7

#include <sal_types.h>
#include <soc/robo2/bcm53158/config.h>
#include <soc/robo2/bcm53158/uart.h>
#include <soc/robo2/bcm53158/init.h>
#include <soc/cpu_m7.h>
#include <sal_libc.h>
#include <sal_console.h>

int int2str(char *buffer, int dec, unsigned num, int size, char pad);

#if 0
/*
 * @brief sysprint_setup
 * Routine to setup uart for early prints
 * Not to be used after console is enabled
 */
void sysprint_setup(uint32 baudrate)
{
    int delay = 10;
    int div = UART_CLOCK / (16 * baudrate);

    /* SET_REG not used due to logging not ready */
    *(volatile uint32*)UART_LCR = 0x83;
    *(volatile uint32*)UART_RBR_THR_DLL = div & 0xFF;
    *(volatile uint32*)UART_DLH_IER = (div >> 8) & 0xFF;
    *(volatile uint32*)UART_LCR = 0x3;
    *(volatile uint32*)UART_MCR = 0x3;
    *(volatile uint32*)UART_IIR_FCR = 0x7;
    while(delay--);
}
#endif

/*
 * @brief sysprint
 * Routine to print directly to uart, used for early print support
 * Not to be used after console is enabled, only console_ routines must be used.
 */
void sysprint(char *buffer)
{
    volatile uint32 *lsr = (volatile uint32*)UART_LSR;
    volatile uint32 *thr = (volatile uint32*)UART_RBR_THR_DLL;
    //volatile uint32 *thr = (volatile uint32*)UART_SRBR_STHR;
    //volatile uint32 *usr = (volatile uint32*)UART_USR;
    while(*buffer) {
        while((*lsr & 0x20) == 0);
        if (*buffer == '\n') {
            *thr = '\r';
            while((*lsr & 0x20) == 0);
        }   
        *thr = *buffer++;
    }
}

/*
 * @brief sysprintf
 * Routine to print formatted string directly to uart, used for early print support
 * Not to be used after console is enabled, only console_ routines must be used.
 */
void sysprintf(char *fmt, ...)
{
    va_list args;
    char buf[256];
    char ch;
    int sz, p = 0;
    char pad = ' ';

#if 0
    sal_memset(buf, 0, 256);
#else
    int i;
    for (i = 0; i< 256; i++) {
        buf[i] = 0;
    }
#endif
    va_start(args, fmt);
    for (ch = *fmt++; ch != '\0'; ch = *fmt++) {
        if (ch != '%') { 
            buf[p++] = ch; 
        }
        else 
        {
            for (sz = 0,ch = *fmt++; ((ch >= '0') && (ch < '9')); ch = *fmt++) {
                if (ch == '0') { 
                    pad = '0';
                    continue;
                }
                sz = sz * 10 + (ch - '0');
                if (sz > 79) {
                    sysprint("Format Spec Size too big\n");
                    return;
                }
            }
            switch (ch) {
            case 'x':
            case 'X':
                 p += int2str(&buf[p], 0, va_arg(args, unsigned int), sz, pad);
                 break;
            case 'u':
                 p += int2str(&buf[p], 1, va_arg(args, unsigned int), sz, pad);
                 break;
            case 'd':
                 p += int2str(&buf[p], 2, va_arg(args, int), sz, pad);
                 break;
            default:
                 sysprint("Unknown format spec\n");
                 return;
            }
        }
        if (p >= 255) {
            buf[255] = '\0';
            break;
        }
    }
    buf[p] = '\0';
    va_end(args);
    sysprint(buf);
}

/*
 * @brief sysputchar
 * Routine to print a char directly to uart
 */
void sysputchar(char data)
{
    volatile uint32 *lsr = (uint32*)UART_LSR;
    volatile uint32 *thr = (uint32*)UART_RBR_THR_DLL;
    while((*lsr & 0x20) == 0);
    *thr = data;
}

/*
 * @brief sysgetchar
 * Routine to read a char directly from uart
 */
char sysgetchar(void)
{
    volatile uint32 *lsr = (uint32*)UART_LSR;
    volatile uint32 *rbr = (uint32*)UART_RBR_THR_DLL;
    while((*lsr & 0x1) == 0);
    return (*rbr);
}

/*
 * @brief syskbhit
 * Routine to know if any key has been pressed
 */
int syskbhit(void)
{
    volatile uint32 *lsr = (uint32*)UART_LSR;
    return ((*lsr & 0x1) == 1);
}

/** 
 * @brief sysassert
 *    Log an assert message 
 */
void sysassert(char *line, char *file, char*cond) 
{
    char buffer[128];
    sprintf(buffer, "**ASSERT FAILED:Line:%s File:%s Conditon:%s\n", line, file, cond);
    sysprint(buffer);
}


/*
 * int2str, mini routine to convert numbers to strings for printing
 * this is intended to be used before the libc is available
 */
int int2str(char *buffer, int dec, unsigned num, int size, char pad)
{
    char decstr[]={ '0','1','2','3','4','5','6','7','8','9'};
    char hexstr[]={ '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    char *p = buffer;
    int div = (dec==0) ? 16 : 10;
    int i, len = 0, npad = 0;
    unsigned tmp = num;
    int tmpi = (int) num;
    if (p) {
        if ((dec == 2) && (tmpi < 0)) {
            *p++ = '-';
            tmp = -tmpi;
        }
        while(tmp) { len++; tmp /= div;}
        if (len == 0) {len++;}
        if (len < size) {
            npad = size - len;
            while(npad--) *p++ = pad;
        }
        for (i=len-1; i>=0; i--) {
            if (dec) {
                *(p+i) = decstr[num % div];
            } else {
                *(p+i) = hexstr[num % div];
            }
            num /= div;
        }
        p += len;
        return (p - buffer);
    }
    return 0;
}

void wait_for_debugger(void)
{
    volatile uint32* cmd = (volatile uint32*)0x4102701c;
    sysprint("Waiting for debugger\n");
    /*
     * Do nothing till debugger gets us out of here 
     * Use debugger to set cmd to non-zero value
     */
    for(*cmd = 0;*cmd == 0; );
}


#endif /* CORTEX_M7 */

