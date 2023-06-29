/*
 * $Copyright: (c) 2017 Broadcom Limited
 */

/*
 * config.h
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define APIFUNC(_fn)    _fn
#define REENTRANT

/* Chip type */

/* Enable console output */
#define CFG_CONSOLE_ENABLED                 (1)

/* Enable debug output */
#define CFG_DEBUGGING_ENABLED               (0)

/* Enable assertion */
#define CFG_ASSERTION_ENABLED               (1)

/* CLI support */
#define CFG_CLI_ENABLED                     (1 && CFG_CONSOLE_ENABLED)

/* CLI prompt */
#define CFG_CLI_PROMPT                      "CMD> "

/* CLI switch commands support */
#define CFG_CLI_SWITCH_CMD_ENABLED          (0)

/* UART baudrate */
#define CFG_UART_BAUDRATE                   (9600)

/* Max background tasks */
#define CFG_MAX_BACKGROUND_TASKS            (2)

#define CFG_PLL2_INIT                       (0)
#define MIB_STAT_ENABLE                     (0)
#define CFG_EMULATOR                        (0)
#define POLLING 0

#define DV_SIM                              (0)
#define DV_SIM_REDUCED_SET                  (0)

#define CFG_TX_ENABLE                       (0)

#endif /* _CONFIG_H_ */
