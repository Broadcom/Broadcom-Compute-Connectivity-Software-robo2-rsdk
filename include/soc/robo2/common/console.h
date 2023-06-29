/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *     console.h
 * Description:
 *     Console defs
 */

#ifndef __CONSOLE_H
#define __CONSOLE_H

extern int console_open(uint32 baudrate);
extern int console_putc(const char ch);
extern int console_puts(const char *buffer);
extern int console_getc(char *ch);
#ifndef WEB_SERVER_SIM
extern inline void console_backspace(int count);
extern inline void console_newline(void);
extern inline int console_kbhit(void);
#endif

#endif
