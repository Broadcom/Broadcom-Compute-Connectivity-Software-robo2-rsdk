/*
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * fsal_ts_cint_data.c
 * 
 * Hand-coded support for FSAL routines.
 */


int fsal_port_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <fsal/ts.h>


extern int ts_event_test(int test_num, int enable);
extern int tse_dump_stats(void);
extern int ts_test(int t1, int t2);
extern int tse_cpu(int num_events);
extern int ts_gen_config_test(int gen_id, uint32_t flags, uint64_t start_sec,
                    uint32_t start_nsec, uint32_t high_nsec, uint32_t low_nsec);
extern int ts_event_config_test(int event_id, int source, uint32_t flags,
                    uint32_t divider);
extern int ts_tod_test(void);


CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                        ts_test,
                        int, int, t1, 0, 0,
                        int, int, t2, 0, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                        ts_event_test,
                        int, int, test_num, 0, 0,
                        int, int, enable, 0, 0);

CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                        tse_dump_stats);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                        tse_cpu,
                        int, int, num_events, 0, 0);

CINT_FWRAPPER_CREATE_RP6(int, int, 0, 0,
                        ts_gen_config_test,
                        int, int, gen_id, 0, 0,
                        uint32_t, uint32_t, flags, 0, 0,
                        uint64_t, uint64_t, start_sec, 0, 0,
                        uint32_t, uint32_t, start_nsec, 0, 0,
                        uint32_t, uint32_t, high_nsec, 0, 0,
                        uint32_t, uint32_t, low_nsec, 0, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                        ts_event_config_test,
                        int, int, event_id, 0, 0,
                        int, int, source, 0, 0,
                        uint32_t, uint32_t, flags, 0, 0,
                        uint32_t, uint32_t, divider, 0, 0);

CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                        ts_tod_test);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_ts_tod_get,
                         cbx_ts_time_t*, cbx_ts_time_t, time, 1, 0,
                         uint64_t*, uint64_t, timestamp, 1, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_ts_tod_set,
                         cbx_ts_time_t*, cbx_ts_time_t, time, 1, 0,
                         uint64_t, uint64_t, timestamp, 0, 0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_ts_event_config_set,
                         int, int, event_id, 0, 0,
                         int, int, event_source, 0, 0,
                         cbx_ts_event_config_t*, cbx_ts_event_config_t, config, 1, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_ts_gen_config_set,
                         int, int, generator_id, 0, 0,
                         cbx_ts_gen_config_t*, cbx_ts_gen_config_t, config, 1, 0);

static void*
__cint_maddr__cbx_ts_time_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_ts_time_t* s = (cbx_ts_time_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->sec); break;
        case 1: rv = &(s->nsec); break;
        default: rv = NULL; break;
    }
    return rv;
}

static void*
__cint_maddr__cbx_ts_event_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_ts_event_t* s = (cbx_ts_event_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->event_id); break;
        case 2: rv = &(s->tod); break;
        default: rv = NULL; break;
    }
    return rv;
}

static void*
__cint_maddr__cbx_ts_event_config_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_ts_event_config_t* s = (cbx_ts_event_config_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->divider); break;
        case 2: rv = &(s->event_cb_func); break;
        default: rv = NULL; break;
    }
    return rv;
}

static void*
__cint_maddr__cbx_ts_gen_config_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_ts_gen_config_t* s = (cbx_ts_gen_config_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->start_time); break;
        case 2: rv = &(s->high_time_nsec); break;
        case 3: rv = &(s->low_time_nsec); break;
        case 4: rv = &(s->start_time); break;
        default: rv = NULL; break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__cbx_ts_time_t[] =
{
    { "uint64_t" , "sec" , 0, 0 },
    { "uint32_t" , "nsec", 0, 0 },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_struct_members__cbx_ts_event_t[] =
{
    { "uint32_t"      , "flags"    , 0, 0 },
    { "uint32_t"      , "event_id" , 0, 0 },
    { "cbx_ts_time_t" , "tod"      , 1, 0 },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_struct_members__cbx_ts_event_config_t[] =
{
    { "uint32_t"          , "flags"         , 0, 0 },
    { "uint32_t"          , "divider"       , 0, 0 },
    { "cbx_ts_event_cb_t" , "event_cb_func" , 1, 0 },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_struct_members__cbx_ts_gen_config_t[] =
{
    { "uint32_t"     , "flags"         , 0, 0 },
    { "cbx_ts_time_t", "start_time"    , 0, 0 },
    { "uint32_t"     , "high_time_nsec", 0, 0 },
    { "uint32_t"     , "low_time_nsec" , 0, 0 },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_fsal_ts_typedefs[] =
{
    CINT_ENTRY_LAST
};


static cint_struct_type_t __cint_fsal_ts_structures[] =
{
    {
    "cbx_ts_time_t",
    sizeof(cbx_ts_time_t),
    __cint_struct_members__cbx_ts_time_t,
    __cint_maddr__cbx_ts_time_t
    },
    {
    "cbx_ts_event_t",
    sizeof(cbx_ts_event_t),
    __cint_struct_members__cbx_ts_event_t,
    __cint_maddr__cbx_ts_event_t
    },
    {
    "cbx_ts_event_config_t",
    sizeof(cbx_ts_event_config_t),
    __cint_struct_members__cbx_ts_event_config_t,
    __cint_maddr__cbx_ts_event_config_t
    },
    {
    "cbx_ts_gen_config_t",
    sizeof(cbx_ts_gen_config_t),
    __cint_struct_members__cbx_ts_gen_config_t,
    __cint_maddr__cbx_ts_gen_config_t
    },
    CINT_ENTRY_LAST
};


static cint_function_t __cint_fsal_ts_functions[] =
    {
        CINT_FWRAPPER_ENTRY(ts_test),
        CINT_FWRAPPER_ENTRY(ts_event_test),
        CINT_FWRAPPER_ENTRY(tse_dump_stats),
        CINT_FWRAPPER_ENTRY(tse_cpu),
        CINT_FWRAPPER_ENTRY(ts_tod_test),
        CINT_FWRAPPER_ENTRY(ts_gen_config_test),
        CINT_FWRAPPER_ENTRY(ts_event_config_test),
        CINT_FWRAPPER_ENTRY(cbx_ts_tod_get),
        CINT_FWRAPPER_ENTRY(cbx_ts_tod_set),
        CINT_FWRAPPER_ENTRY(cbx_ts_event_config_set),
        CINT_FWRAPPER_ENTRY(cbx_ts_gen_config_set),
        CINT_ENTRY_LAST
    };


static cint_enum_type_t __cint_fsal_ts_enums[] =
{
    CINT_ENTRY_LAST
};


static cint_constants_t __cint_fsal_ts_constants[] =
{
    {"CBX_TS_EVENT_ID_CPU", CBX_TS_EVENT_ID_CPU},
    {"CBX_TS_EVENT_ID_GPIO0", CBX_TS_EVENT_ID_GPIO0},
    {"CBX_TS_EVENT_ID_GPIO1", CBX_TS_EVENT_ID_GPIO1},
    {"CBX_TS_EVENT_ID_GPIO2", CBX_TS_EVENT_ID_GPIO2},
    {"CBX_TS_EVENT_ID_GPIO3", CBX_TS_EVENT_ID_GPIO3},
    {"CBX_TS_EVENT_ID_GPIO4", CBX_TS_EVENT_ID_GPIO4},
    {"CBX_TS_EVENT_ID_GPIO5", CBX_TS_EVENT_ID_GPIO5},
    {"CBX_TS_EVENT_ID_GPIO6", CBX_TS_EVENT_ID_GPIO6},
    {"CBX_TS_EVENT_ID_GPIO7", CBX_TS_EVENT_ID_GPIO7},
    {"CBX_TS_EVENT_ID_SYNCE_CLK0", CBX_TS_EVENT_ID_SYNCE_CLK0},
    {"CBX_TS_EVENT_ID_SYNCE_CLK1", CBX_TS_EVENT_ID_SYNCE_CLK1},
    {"CBX_TS_EVENT_ID_SYNCE_CLK2", CBX_TS_EVENT_ID_SYNCE_CLK2},
    {"CBX_TS_EVENT_ID_SYNCE_CLK3", CBX_TS_EVENT_ID_SYNCE_CLK3},
    {"CBX_TS_EVENT_ID_SYNCE_CLK4", CBX_TS_EVENT_ID_SYNCE_CLK4},
    {"CBX_TS_EVENT_ID_SYNCE_CLK5", CBX_TS_EVENT_ID_SYNCE_CLK5},
    {"CBX_TS_EVENT_ID_SYNC_IN0", CBX_TS_EVENT_ID_SYNC_IN0},
    {"CBX_TS_EVENT_ID_SYNC_IN1", CBX_TS_EVENT_ID_SYNC_IN1},
    {"CBX_TS_EVENT_ID_SYNC_IN2", CBX_TS_EVENT_ID_SYNC_IN2},
    {"CBX_TS_EVENT_ID_SYNC_IN3", CBX_TS_EVENT_ID_SYNC_IN3},

    {"CBX_TS_EVENT_SOURCE_GPHY0_CLK0", CBX_TS_EVENT_SOURCE_GPHY0_CLK0},
    {"CBX_TS_EVENT_SOURCE_GPHY0_CLK1", CBX_TS_EVENT_SOURCE_GPHY0_CLK1},
    {"CBX_TS_EVENT_SOURCE_GPHY0_CLK2", CBX_TS_EVENT_SOURCE_GPHY0_CLK2},
    {"CBX_TS_EVENT_SOURCE_GPHY0_CLK3", CBX_TS_EVENT_SOURCE_GPHY0_CLK3},
    {"CBX_TS_EVENT_SOURCE_GPHY1_CLK0", CBX_TS_EVENT_SOURCE_GPHY1_CLK0},
    {"CBX_TS_EVENT_SOURCE_GPHY1_CLK1", CBX_TS_EVENT_SOURCE_GPHY1_CLK1},
    {"CBX_TS_EVENT_SOURCE_GPHY1_CLK2", CBX_TS_EVENT_SOURCE_GPHY1_CLK2},
    {"CBX_TS_EVENT_SOURCE_GPHY1_CLK3", CBX_TS_EVENT_SOURCE_GPHY1_CLK3},
    {"CBX_TS_EVENT_SOURCE_XFI0_CLK", CBX_TS_EVENT_SOURCE_XFI0_CLK},
    {"CBX_TS_EVENT_SOURCE_XFI1_CLK", CBX_TS_EVENT_SOURCE_XFI1_CLK},
    {"CBX_TS_EVENT_SOURCE_RECOV_CLK0", CBX_TS_EVENT_SOURCE_RECOV_CLK0},
    {"CBX_TS_EVENT_SOURCE_RECOV_CLK1", CBX_TS_EVENT_SOURCE_RECOV_CLK1},
    {"CBX_TS_EVENT_SOURCE_RECOV_CLK2", CBX_TS_EVENT_SOURCE_RECOV_CLK2},
    {"CBX_TS_EVENT_SOURCE_TS_TIMER0", CBX_TS_EVENT_SOURCE_TS_TIMER0},
    {"CBX_TS_EVENT_SOURCE_TS_TIMER1", CBX_TS_EVENT_SOURCE_TS_TIMER1},
    {"CBX_TS_EVENT_SOURCE_LCPLL", CBX_TS_EVENT_SOURCE_LCPLL},
    {"CBX_TS_EVENT_SOURCE_SYNC_IN", CBX_TS_EVENT_SOURCE_SYNC_IN},
    {"CBX_TS_EVENT_SOURCE_SYNC_OUT", CBX_TS_EVENT_SOURCE_SYNC_OUT},
    {"CBX_TS_EVENT_SOURCE_FRAME_SYNC_IN", CBX_TS_EVENT_SOURCE_FRAME_SYNC_IN},
    {"CBX_TS_EVENT_SOURCE_FRAME_SYNC_OUT", CBX_TS_EVENT_SOURCE_FRAME_SYNC_OUT},

    { "CBX_TS_EVENT_CONFIG_F_ENABLE", CBX_TS_EVENT_CONFIG_F_ENABLE},
    { "CBX_TS_EVENT_CONFIG_F_DIVIDER", CBX_TS_EVENT_CONFIG_F_DIVIDER},
    { "CBX_TS_EVENT_CONFIG_F_CB", CBX_TS_EVENT_CONFIG_F_CB},
    { "CBX_TS_EVENT_CONFIG_F_FORCE_VALID", CBX_TS_EVENT_CONFIG_F_FORCE_VALID},
    { "CBX_TS_EVENT_CONFIG_F_HW_DPLL", CBX_TS_EVENT_CONFIG_F_HW_DPLL},
    { "CBX_TS_EVENT_CONFIG_F_CUST_1", CBX_TS_EVENT_CONFIG_F_CUST_1},
    { "CBX_TS_EVENT_CONFIG_F_CUST_2", CBX_TS_EVENT_CONFIG_F_CUST_2},

    { "CBX_TS_GEN_CONFIG_F_ENABLE", CBX_TS_GEN_CONFIG_F_ENABLE},
    { "CBX_TS_GEN_CONFIG_F_START_TIME", CBX_TS_GEN_CONFIG_F_START_TIME},
    { "CBX_TS_GEN_CONFIG_F_ONE_SHOT", CBX_TS_GEN_CONFIG_F_ONE_SHOT},

    { "CBX_TS_GEN_ID_TAS_1588_SYNC_A", CBX_TS_GEN_ID_TAS_1588_SYNC_A},
    { "CBX_TS_GEN_ID_TAS_1588_SYNC_B", CBX_TS_GEN_ID_TAS_1588_SYNC_B},
    { "CBX_TS_GEN_ID_2", CBX_TS_GEN_ID_2},
    { "CBX_TS_GEN_ID_TAS_1588_CLK", CBX_TS_GEN_ID_TAS_1588_CLK},
    { "CBX_TS_GEN_ID_TS_SYNC_O", CBX_TS_GEN_ID_TS_SYNC_O},
    { "CBX_TS_GEN_ID_TS_FRAME_SYNC_O", CBX_TS_GEN_ID_TS_FRAME_SYNC_O},
    { "CBX_TS_GEN_ID_LAST", CBX_TS_GEN_ID_LAST},
    CINT_ENTRY_LAST
};


static cint_function_pointer_t __cint_function_pointers[2];
static int
__cint_fpointer__event_cb_func(int event_id, cbx_ts_event_t ts_event)
{
  int returnval;

  cint_interpreter_callback(__cint_function_pointers+0, 2, 1, &event_id, &ts_event, &returnval);

  return returnval;
}
static cint_parameter_desc_t __cint_parameters_event_cb_func[] =
{
    {
        "int",
        "r",
        0,
        0
    },
    {
        "int",
        "event_id",
        0,
        0
    },
    {
        "cbx_ts_event_t",
        "ts_event",
        1,
        0
    },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_ts_function_pointers[] =
{
    {
        "cbx_ts_event_cb_t",
        (cint_fpointer_t) __cint_fpointer__event_cb_func,
        __cint_parameters_event_cb_func,
    },
    CINT_ENTRY_LAST
};

cint_data_t fsal_ts_cint_data =
    {
        NULL,
        __cint_fsal_ts_functions,
    __cint_fsal_ts_structures,
    __cint_fsal_ts_enums,
    __cint_fsal_ts_typedefs,
    __cint_fsal_ts_constants,
    __cint_fsal_ts_function_pointers,
    };




#endif /* INCLUDE_LIB_CINT */
