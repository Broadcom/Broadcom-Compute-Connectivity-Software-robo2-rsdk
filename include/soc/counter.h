/*
 * $ID:$
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 * File:
 *    counter.h
 * Description
 *    counter.h
 */

#ifndef __COUNTER_H
#define __COUNTER_H

#include <soc/defs.h>
#include <soc/types.h>
#include <sal_types.h>
#include <soc/robo2/common/allenum.h>

#define SOC_REG_IS_COUNTER(unit, socreg) \
        ((SOC_REG_INFO(unit, socreg).flags & SOC_REG_FLAG_COUNTER) && \
         (SOC_REG_INFO(unit, socreg).regtype != soc_cpureg))

#define SOC_COUNTER_INVALID(unit, reg) \
        ((reg == INVALIDr))

/*
 * Counter map structure.
 * Each chip has an array of these structures indexed by soc_ctr_type_t
 */

typedef struct soc_ctr_ref_s {
    soc_reg_t  reg;
    int        index;
} soc_ctr_ref_t;

typedef struct soc_cmap_s {
    soc_ctr_ref_t  *cmap_base;    /* The array of counters as they are DMA'd */
    int                cmap_size;    /* The number of elements in the array */
} soc_cmap_t;

extern int soc_port_stat_dump(uint32_t portid);
extern int soc_port_stat_clear(uint32_t portid);
extern int
soc_robo_counter_set_by_port(int unit, uint32_t port, uint64_t val);
extern int soc_robo_counter_start(int unit, uint32 flags, int interval, pbmp_t pbmp);
extern int soc_robo_counter_sync(int unit);
extern int soc_robo_counter_stop(int unit);

#endif /* __COUNTER_H */
