/*
 * $ID: $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:        cmd_cint.h
 *
 * Note - this file needs to be includable by cint_yy.h, so cannot include any
 * SDK interfaces directly.
 *
 */

#ifndef   _CMD_CINT_H_
#define   _CMD_CINT_H_

#include <stdio.h>
extern int cmd_cint_initialize(void);
extern void cmd_cint_fatal_error(char *msg);
#include <bsl.h>
#include <cint_types.h>
#include <cint_interpreter.h>

#if !defined(__KERNEL__) && !defined(VXWORKS)
#include <stdlib.h>
#endif

#include <sal_libc.h>
#include <sal_console.h>
#include <cint_porting.h>
#include <defs.h>

char cmd_cint_usage[] = "cint usage goes here\n";


#endif /* _CMD_CINT_H_ */

