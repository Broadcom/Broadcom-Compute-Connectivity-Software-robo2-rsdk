/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * Broadcom System Log Enables
 */

#include <string.h>
#include "bslenable.h"

/* Table for tracking which layers/sources/severities to enable */
static bsl_severity_t bslenable_severity[bslLayerCount][bslSourceCount];

/* Which sources are valid in which layers (fixed) */
BSL_SOURCES_VALID_DEF(bsl_source_t);
static bsl_source_t *sources_valid[] = BSL_SOURCES_VALID_INIT;

int
bslenable_source_valid(bsl_layer_t layer, bsl_source_t source)
{
    bsl_source_t *src;
    int idx;

    if (layer < 0 || layer >= bslLayerCount) {
        return 0;
    }
    if (source < 0 || source >= bslSourceCount) {
        return 0;
    }
    src = sources_valid[layer];
    for (idx = 0; src[idx] != bslSourceCount && idx < bslSourceCount; idx++) {
        if (source == src[idx]) {
            return 1;
        }
    }
    return 0;
}

void
bslenable_set(bsl_layer_t layer, bsl_source_t source, bsl_severity_t severity)
{
    if (layer < 0 || layer >= bslLayerCount) {
        return;
    }
    bslenable_severity[layer][source] = severity;
}

bsl_severity_t
bslenable_get(bsl_layer_t layer, bsl_source_t source)
{
    if (layer < bslLayerCount && source < bslSourceCount) {
        return bslenable_severity[layer][source];
    }
    return 0;
}

void
bslenable_reset(bsl_layer_t layer, bsl_source_t source)
{
    bsl_severity_t severity = bslSeverityWarn;
    bslenable_set(layer, source, severity);
}

void
bslenable_reset_all(void)
{
    int layer, source;

    for (layer = 0; layer < bslLayerCount; layer++) {
        for (source = 0; source < bslSourceCount; source++) {
            bslenable_reset(layer, source);
        }
    }
}

int
bslenable_init(void)
{
    bslenable_reset_all();

    return 0;
}

/* "Fast" checker function */
int
bsl_fast_check(unsigned int chk)
{
    int layer, source, severity;

    layer = BSL_LAYER_GET(chk);
    source = BSL_SOURCE_GET(chk);
    severity = BSL_SEVERITY_GET(chk);

    return (severity <= bslenable_get(layer, source));
}

