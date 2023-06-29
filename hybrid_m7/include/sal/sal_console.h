/*
 * $Copyright: (c) 2017 Broadcom Limited
 */

/*
 * $ sal_console.h $
 */

#ifndef _SAL_CONSOLE_H_
#define _SAL_CONSOLE_H_

extern void sal_assert(const char *, const char *, uint16) REENTRANT;

#if CFG_ASSERTION_ENABLED
#define SAL_ASSERT(x) (void)((x)? 0 : (sal_assert(#x, __FILE__, __LINE__), 0))
#else
#define SAL_ASSERT(x) do { } while(0)
#endif


/* Console - printf */
extern void sal_printf(const char *fmt, ...) REENTRANT;

/* Console - get input character (may block if no char available) */
extern char sal_getchar(void) REENTRANT;

/* Console - return the last inputed character */
extern char sal_get_last_char(void) REENTRANT;

/* Console - output one character */
extern char sal_putchar(char c) REENTRANT;


/* Debugging */
#if CFG_DEBUGGING_ENABLED
extern BOOL is_debug_flag_set(uint16 flag); 
#define DBG_FLAG_LINK_SCAN  0x0001
#define DBG_FLAG_PORT       0x0002
#define DBG_FLAG_BACKGROUND 0x0004
#define DBG_FLAG_PHY        0x0008
#define DBG_FLAG_EEPROM     0x0010
#define DBG_FLAG_INIT       0x0020
#define DBG_FLAG_LED        0x0040
#define DBG_FLAG_AVS        0x0080
#define DBG_FLAG_CONFIG_CMD 0x0100
#define DBG_FLAG_PKTIO      0x0200

#if DV_SIM_REDUCED_SET
#define DBG_LOG(f,x)
#else /* DV_SIM_REDUCED_SET */
#define DBG_LOG(f, x)                        \
          do {                               \
              if(is_debug_flag_set(f)) {     \
                  sal_printf x;              \
              }                              \
          } while(0)
#endif /* DV_SIM_REDUCED_SET */
#else /* CFG_DEBUGGING_ENABLED */

#define DBG_LOG(f,x)

#endif


#define isdigit(d)      (((d) >= '0') && ((d) <= '9'))
#define isxdigit(c)      ((((c) >= '0') && ((c) <= '9')) || \
                            (((c) >= 'a') && ((c) <= 'f')) || \
                            (((c) >= 'A') && ((c) <= 'F')))
#define isspace(x)      (((x) == ' ') || ((x) == '\t'))

/* C library */
#ifdef __C51__

extern int sal_strlen(const char *s) REENTRANT;
extern char *sal_strchr(const char *dest,int c) REENTRANT;

#endif /* __C51__ */


#endif /* _SAL_CONSOLE_H_ */
