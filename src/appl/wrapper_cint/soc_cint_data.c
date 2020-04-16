/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * soc_cint_data.c
 *
 * Hand-coded support for a few SAL core routines.
 */
int soc_core_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <drv.h>
#include <counter.h>

extern int soc_phy_probe(void);
extern void phy_debug_set(int enable);
extern void set_skip_pbmp(uint32 bip);
extern void cutthru_enable(void);
extern void cutthru_disable(void);


CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_init,
                         int,int,unit,0,0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_port_stat_clear,
                         uint32, uint32, portid, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_port_stat_dump,
                         uint32, uint32, portid, 0, 0);
CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         soc_pkt_send,
                         uint32, uint32, port, 0, 0,
                         int, int, vlan, 0, 0,
                         uint32, uint32, mgid, 0, 0,
                         int, int, type, 0, 0,
                         int, int, mode, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         soc_pktio_init,
                         int,int,unit,0,0);
CINT_FWRAPPER_CREATE_VP1(dump_hpa_regs,
                         int,int,unit,0,0);
CINT_FWRAPPER_CREATE_VP1(dump_cb_counters,
                         int,int,unit,0,0);
CINT_FWRAPPER_CREATE_VP1(dump_port_status,
                         int,int,unit,0,0);
CINT_FWRAPPER_CREATE_VP1(dump_l2_table,
                         int, int, unit, 0, 0);
CINT_FWRAPPER_CREATE_VP0(sal_print_mem_stats);
CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                         soc_phy_probe);
CINT_FWRAPPER_CREATE_VP1(phy_debug_set,
                         int,int,enable,0,0);
CINT_FWRAPPER_CREATE_VP1(set_skip_pbmp,
                         uint32,uint32,bitmap,0,0);
CINT_FWRAPPER_CREATE_VP0(cutthru_enable);
CINT_FWRAPPER_CREATE_VP0(cutthru_disable);

static cint_struct_type_t __cint_soc_structures[] =
{
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_soc_enums[] =
{
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_soc_typedefs[] =
{
    {
        "int",
        "soc_port_t",
        0,
        0
    },
    {NULL}
};

static cint_constants_t __cint_soc_constants[] =
{
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_soc_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_soc_functions[] =
    {
        CINT_FWRAPPER_ENTRY(soc_init),
        CINT_FWRAPPER_ENTRY(soc_port_stat_clear),
        CINT_FWRAPPER_ENTRY(soc_port_stat_dump),
        CINT_FWRAPPER_ENTRY(soc_pkt_send),
        CINT_FWRAPPER_ENTRY(soc_pktio_init),
        CINT_FWRAPPER_ENTRY(dump_hpa_regs),
        CINT_FWRAPPER_ENTRY(dump_cb_counters),
        CINT_FWRAPPER_ENTRY(dump_port_status),
        CINT_FWRAPPER_ENTRY(dump_l2_table),
        CINT_FWRAPPER_ENTRY(sal_print_mem_stats),
        CINT_FWRAPPER_ENTRY(soc_phy_probe),
        CINT_FWRAPPER_ENTRY(phy_debug_set),
        CINT_FWRAPPER_ENTRY(set_skip_pbmp),
        CINT_FWRAPPER_ENTRY(cutthru_enable),
        CINT_FWRAPPER_ENTRY(cutthru_disable),
        CINT_ENTRY_LAST
    };
cint_data_t soc_cint_data =
    {
        NULL,
        __cint_soc_functions,
        __cint_soc_structures,
        __cint_soc_enums,
        __cint_soc_typedefs,
        __cint_soc_constants,
        __cint_soc_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */

