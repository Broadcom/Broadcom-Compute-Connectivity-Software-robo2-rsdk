/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * Broadcom System Log State
 */

#ifndef _DIAG_BSLENABLE_H
#define _DIAG_BSLENABLE_H

#include "bslenum.h"

#if 0
#define bslmgmt_source_valid    bslenable_source_valid
#define bslmgmt_set             bslenable_set
#define bslmgmt_get             bslenable_get
#define bslmgmt_reset           bslenable_reset
#endif

extern int
bslenable_source_valid(bsl_layer_t layer, bsl_source_t source);

extern void
bslenable_set(bsl_layer_t layer, bsl_source_t source, bsl_severity_t severity);

extern bsl_severity_t
bslenable_get(bsl_layer_t layer, bsl_source_t source);

extern void
bslenable_reset(bsl_layer_t layer, bsl_source_t source);

extern void
bslenable_reset_all(void);

extern int
bsl_fast_check(unsigned int chk);

extern int
bslenable_init(void);

#endif /* !_DIAG_BSLENABLE_H */
