/*
 * $Copyright: (c) 2017 Broadcom Limited.
 * All Rights Reserved.$
 * File:
 *    robo2.h
 * Description
 *    robo2.h
 */

#ifndef _SOC_ROBO2_H_
#define _SOC_ROBO2_H_

#include <soc/drv.h>

#define SOC_ROBO2_PORT_INFO(unit, port)  (soc_robo_port_info[(unit)][(port)])
#define SOC_ROBO2_PORT_MEDIUM_MODE_SET(unit, port, medium)   \
    (SOC_ROBO2_PORT_INFO(unit, port).cur_medium = (medium))
#define SOC_ROBO2_PORT_MEDIUM_MODE(unit, port)   \
    (SOC_ROBO2_PORT_INFO(unit, port).cur_medium)
#define SOC_IS_RORO_PORT_MEDIUM_MODE_COPPER(unit, port) \
    SOC_ROBO2_PORT_MEDIUM_MODE((unit),(port)) == (SOC_PORT_MEDIUM_COPPER) ?\
    TRUE : FALSE
#define SOC_ROBO2_PORT_MAC_DRIVER(unit, port)    \
    (SOC_ROBO2_PORT_INFO(unit, port).p_mac)
#define SOC_ROBO2_PORT_INIT(unit) \
    if (soc_robo_port_info[unit] == NULL) { return SOC_E_INIT; }

typedef struct soc_robo2_port_info_s {
#if 0 /* Enable after MAC driver is added */
    mac_driver_t    *p_mac;     /* Per port MAC driver */
#endif
    int            p_ut_prio;    /* Untagged priority */

    uint8   cur_medium;     /* current medium (copper/fiber) */

    uint32 ing_sample_rate; /* ingress sample rate */
    uint32 eg_sample_rate; /* egress sample rate */
    uint32 ing_sample_prio; /* priority of ingress sample packets */
    uint32 eg_sample_prio; /* priority of egress sample packets */
} soc_robo2_port_info_t;

extern soc_robo2_port_info_t *soc_robo2_port_info[SOC_MAX_NUM_DEVICES];

extern int soc_robo2_misc_init(int);
extern int soc_robo2_mmu_init(int);
extern int soc_robo2_age_timer_get(int, int *, int *);
extern int soc_robo2_age_timer_max_get(int, int *);
extern int soc_robo2_age_timer_set(int, int, int);
#if 0
extern int soc_robo_64_val_to_pbmp(int, soc_pbmp_t *, uint64);
extern int soc_robo_64_pbmp_to_val(int , soc_pbmp_t *, uint64 *);
#endif

extern soc_functions_t soc_robo2_drv_funs;
extern void soc_robo2_counter_thread_run_set(int run);

#endif  /* !_SOC_ROBO2_H_ */
