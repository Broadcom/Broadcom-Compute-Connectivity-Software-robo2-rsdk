/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 */

#ifndef _SAL_CONSOLE_H_
#define _SAL_CONSOLE_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sal_libc.h>
#include <sal_types.h>


/* Debugging */
#if CFG_DEBUGGING_ENABLED
extern void sal_debugf(const char *fmt, ...);
#define SAL_DEBUGF(x) do { sal_debugf x; } while(0)
#else
#define SAL_DEBUGF(x) do { } while(0)
#endif

/* Assertion */
extern void sal_assert(const char *, const char *, uint16);
#if CFG_ASSERTION_ENABLED
#define SAL_ASSERT(x) (void)((x)? 0 : (sal_assert(#x, __FILE__, __LINE__), 0))
#else
#define SAL_ASSERT(x) do { } while(0)
#endif


/* Console - whether if char is available for input */
extern BOOL sal_char_avail(void);

/* Console - get input character (may block if no char available) */
extern char sal_getchar(void);
extern char my_getchar(void *);

/* Console - return the last inputed character */
extern char sal_get_last_char(void);

/* Console - output one character */
extern char sal_putchar(char c);
extern void sal_backspace(int count);

/* Console - printf */
extern int sal_printf(const char *fmt, ...);
extern int sal_vprintf(const char *fmt, va_list args);

extern int sal_sprintf(char *buf,const char *templat,...);
extern int sal_snprintf(char *buf,size_t buf_size,const char *templat,...);
extern int sal_vsprintf(char *outbuf,const char *templat,va_list marker);
extern int sal_vsnprintf(char *outbuf,size_t buf_size,const char *templat,
							va_list marker);

extern int sal_sscanf(const char *input, const char *format, ...);

#endif /* _SAL_CONSOLE_H_ */
