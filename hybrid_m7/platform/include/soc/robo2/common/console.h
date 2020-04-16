/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
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
extern int console_putc(const uint8 ch);
extern int console_puts(const uint8 *buffer);
extern int console_getc(uint8 *ch);
#ifndef WEB_SERVER_SIM
extern inline void console_backspace(int count);
extern inline void console_newline(void);
extern inline int console_kbhit(void);
#endif

#endif
