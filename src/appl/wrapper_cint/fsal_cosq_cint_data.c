/*
 *
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * fsal_cosq_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_cosq_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <cosq.h>

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_create,
                         cbx_cosq_params_t *, cbx_cosq_params_t,
                         cosq_params, 1, 0,
                         cbx_cosqid_t *, cbx_cosqid_t, cosqid, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_cosq_destroy,
                         cbx_cosqid_t, cbx_cosqid_t, cosqid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_port_default_tc_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
						 cbx_tc_t, cbx_tc_t, tc, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_port_default_tc_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
						 cbx_tc_t *, cbx_tc_t, tc, 1, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_cosq_port_default_pcp_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
						 int, int, pcp, 0, 0,
						 int, int, dei, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_cosq_port_default_pcp_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
						 int *, int, pcp, 1, 0,
						 int *, int, dei, 1, 0);
CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         cbx_cosq_port_pcp2tc_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
						 int, int, pcp, 0, 0,
						 int, int, dei, 0, 0,
						 cbx_tc_t ,cbx_tc_t, tc, 0, 0);
CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         cbx_cosq_port_pcp2tc_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
						 int, int, pcp, 0, 0,
						 int, int, dei, 0, 0,
						 cbx_tc_t *,cbx_tc_t, tc, 1, 0);
CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         cbx_cosq_port_tc2pcp_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
						 cbx_tc_t ,cbx_tc_t, tc, 0, 0,
						 int, int, pcp, 0, 0,
						 int, int, dei, 0, 0);
CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         cbx_cosq_port_tc2pcp_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
						 cbx_tc_t ,cbx_tc_t, tc, 0, 0,
						 int *, int, pcp, 1, 0,
						 int *, int, dei, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_dscp2tc_set,
                         cbx_dscp_t, cbx_dscp_t, dscp, 0, 0,
						 cbx_tc_t, cbx_tc_t, tc, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_dscp2tc_get,
                         cbx_dscp_t, cbx_dscp_t, dscp, 0, 0,
						 cbx_tc_t *, cbx_tc_t, tc, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_tc2dscp_set,
						 cbx_tc_t, cbx_tc_t, tc, 0, 0,
                         cbx_dscp_t, cbx_dscp_t, dscp, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_tc2dscp_get,
						 cbx_tc_t, cbx_tc_t, tc, 0, 0,
                         cbx_dscp_t *, cbx_dscp_t, dscp, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_port_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_cosq_params_t *, cbx_cosq_params_t,
                         cosq_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_port_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_cosq_params_t , cbx_cosq_params_t,
                         cosq_params, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_cosq_port_sched_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_schedule_mode_t *, cbx_schedule_mode_t,
                         sched_mode, 1, 0,
                         int *, int, weights, 1, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_cosq_port_sched_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_schedule_mode_t, cbx_schedule_mode_t,
                         sched_mode, 0, 0,
                         int *, int, weights, 1, 0);
CINT_FWRAPPER_CREATE_VP2( cbx_flowcontrol_params_t_init,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_flowcontrol_params_t *, cbx_flowcontrol_params_t,
                         flowcontrol_params, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_cosq_flowcontrol_get,
                         cbx_flowcontrol_t *, cbx_flowcontrol_t,
                         flowcontrol_mode, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_cosq_flowcontrol_set,
                         cbx_flowcontrol_t, cbx_flowcontrol_t,
                         flowcontrol_mode, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_port_flowcontrol_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_flowcontrol_params_t *, cbx_flowcontrol_params_t,
                         fc_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_port_flowcontrol_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_flowcontrol_params_t, cbx_flowcontrol_params_t,
                         fc_params, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_pfc_flowcontrol_get,
                         int, int, int_pri, 0, 0,
                         cbx_flowcontrol_params_t *, cbx_flowcontrol_params_t,
                         fc_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_pfc_flowcontrol_set,
                         int, int, int_pri, 0, 0,
                         cbx_flowcontrol_params_t, cbx_flowcontrol_params_t,
                         fc_params, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_port_shaper_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_cosq_shaper_params_t *, cbx_cosq_shaper_params_t,
                         shaper_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cosq_port_shaper_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_cosq_shaper_params_t *, cbx_cosq_shaper_params_t,
                         shaper_params, 1, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_cosq_queue_shaper_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         int, int, int_pri, 0, 0,
                         cbx_cosq_shaper_params_t *, cbx_cosq_shaper_params_t,
                         shaper_params, 1, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_cosq_queue_shaper_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         int, int, int_pri, 0, 0,
                         cbx_cosq_shaper_params_t *, cbx_cosq_shaper_params_t,
                         shaper_params, 1, 0);
#ifdef CONFIG_TIMESYNC
CINT_FWRAPPER_CREATE_VP1(cbx_cosq_tsn_params_t_init,
                         cbx_cosq_tsn_params_t *, cbx_cosq_tsn_params_t,
                         tsn_params, 1, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_cosq_queue_tsn_config_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         int, int, int_pri, 0, 0,
                         cbx_cosq_tsn_params_t *, cbx_cosq_tsn_params_t,
                         tsn_params, 1, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_cosq_queue_tsn_config_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         int, int, int_pri, 0, 0,
                         cbx_cosq_tsn_params_t *, cbx_cosq_tsn_params_t,
                         tsn_params, 1, 0);
#endif

static void*
__cint_maddr__cbx_cosq_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_cosq_params_t* s = (cbx_cosq_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->en_tc); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_tc_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_tc_t* s = (cbx_tc_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->int_pri); break;
        case 1: rv = &(s->int_dp); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_dscp_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_dscp_t* s = (cbx_dscp_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->dscp_cp); break;
        case 1: rv = &(s->is_ipv6); break;
        default: rv = NULL; break;
    }
    return rv;
}

static void*
__cint_maddr__cbx_flowcontrol_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_flowcontrol_params_t* s = (cbx_flowcontrol_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->enable); break;
        case 1: rv = &(s->thresh3); break;
        case 2: rv = &(s->thresh2); break;
        case 3: rv = &(s->thresh1); break;
        case 4: rv = &(s->thresh0); break;
        case 5: rv = &(s->hysteresis); break;
        default: rv = NULL; break;
    }
    return rv;
}

static void*
__cint_maddr__cbx_cosq_shaper_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_cosq_shaper_params_t* s = (cbx_cosq_shaper_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->bits_sec); break;
        case 2: rv = &(s->max_bits_sec); break;
        case 3: rv = &(s->bits_burst); break;
        case 4: rv = &(s->kbits_current); break;
        default: rv = NULL; break;
    }
    return rv;
}

#ifdef CONFIG_TIMESYNC
static void*
__cint_maddr__cbx_cosq_tsn_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_cosq_tsn_params_t* s = (cbx_cosq_tsn_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->cfg_id); break;
        case 1: rv = &(s->flags); break;
        case 2: rv = &(s->sync_source); break;
        case 3: rv = &(s->tick_period); break;
        case 4: rv = &(s->start_state); break;
        case 5: rv = &(s->start_duration); break;
        case 6: rv = &(s->gb_duration); break;
        case 7: rv = &(s->hp_duration); break;
        case 8: rv = &(s->lp_duration); break;
        default: rv = NULL; break;
    }
    return rv;
}
#endif

static cint_parameter_desc_t __cint_struct_members__cbx_cosq_params_t[] =
{
    {
        "int",
        "en_tc",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_tc_t[] =
{
    {
        "int",
        "int_pri",
        0,
        0
    },
    {
        "cbx_dp_t",
        "int_dp",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_dscp_t[] =
{
    {
        "int",
        "dscp_cp",
        0,
        0
    },
    {
        "cbx_dp_t",
        "is_ipv6",
        0,
        0
    },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_struct_members__cbx_flowcontrol_params_t[] =
{
    {
        "int",
        "enable",
        0,
        0
    },
    {
        "int",
        "thresh3",
        0,
        0
    },
    {
        "int",
        "thresh2",
        0,
        0
    },
    {
        "int",
        "thresh1",
        0,
        0
    },
    {
        "int",
        "thresh0",
        0,
        0
    },
    {
        "int",
        "hysteresis",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_struct_members__cbx_cosq_shaper_params_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "uint32_t",
        "bits_sec",
        0,
        0
    },
    {
        "uint32_t",
        "max_bits_sec",
        0,
        0
    },
    {
        "uint32_t",
        "bits_burst",
        0,
        0
    },
    {
        "uint32_t",
        "kbits_current",
        0,
        0
    },
    CINT_ENTRY_LAST
};

#ifdef CONFIG_TIMESYNC
static cint_parameter_desc_t __cint_struct_members__cbx_cosq_tsn_params_t[] =
{
    {
        "int",
        "cfg_id",
        0,
        0
    },
    {
        "int",
        "flags",
        0,
        0
    },
    {
        "int",
        "sync_source",
        0,
        0
    },
    {
        "int",
        "tick_period",
        0,
        0
    },
    {
        "int",
        "start_state",
        0,
        0
    },
    {
        "int",
        "start_duration",
        0,
        0
    },
    {
        "int",
        "gb_duration",
        0,
        0
    },
    {
        "int",
        "hp_duration",
        0,
        0
    },
    {
        "int",
        "lp_duration",
        0,
        0
    },
    CINT_ENTRY_LAST
};
#endif

static cint_struct_type_t __cint_fsal_cosq_structures[] =
{
    {
    "cbx_cosq_params_t",
    sizeof(cbx_cosq_params_t),
    __cint_struct_members__cbx_cosq_params_t,
    __cint_maddr__cbx_cosq_params_t
    },
    {
    "cbx_tc_t",
    sizeof(cbx_tc_t),
    __cint_struct_members__cbx_tc_t,
    __cint_maddr__cbx_tc_t
    },
    {
    "cbx_dscp_t",
    sizeof(cbx_dscp_t),
    __cint_struct_members__cbx_dscp_t,
    __cint_maddr__cbx_dscp_t
    },
    {
    "cbx_flowcontrol_params_t",
    sizeof(cbx_flowcontrol_params_t),
    __cint_struct_members__cbx_flowcontrol_params_t,
    __cint_maddr__cbx_flowcontrol_params_t
    },
    {
    "cbx_cosq_shaper_params_t",
    sizeof(cbx_cosq_shaper_params_t),
    __cint_struct_members__cbx_cosq_shaper_params_t,
    __cint_maddr__cbx_cosq_shaper_params_t
    },
#ifdef CONFIG_TIMESYNC
    {
    "cbx_cosq_tsn_params_t",
    sizeof(cbx_cosq_tsn_params_t),
    __cint_struct_members__cbx_cosq_tsn_params_t,
    __cint_maddr__cbx_cosq_tsn_params_t
    },
#endif
    CINT_ENTRY_LAST
};



static cint_enum_map_t __cint_enum_map__cbx_dp_t[] =
{
    { "cbxDpGreen", cbxDpGreen },
    { "cbxDpYellow", cbxDpYellow },
    { "cbxDpRed", cbxDpRed },
    { "cbxDpDrop", cbxDpDrop },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_schedule_mode_t[] =
{
    { "cbxScheduleStrict", cbxScheduleStrict },
    { "cbxScheduleRR", cbxScheduleRR },
    { "cbxScheduleWRR", cbxScheduleWRR },
    { "cbxScheduleWDRR", cbxScheduleWDRR },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_flowcontrol_t[] =
{
    { "cbxFlowcontrolNone", cbxFlowcontrolNone },
    { "cbxFlowcontrolPause", cbxFlowcontrolPause },
    { "cbxFlowcontrolPFC", cbxFlowcontrolPFC },
    { "cbxFlowcontrolMeter", cbxFlowcontrolMeter },
    CINT_ENTRY_LAST
};

#ifdef CONFIG_TIMESYNC
static cint_enum_map_t __cint_enum_map__cbx_tsn_state_t[] =
{
    { "cbxTsnStateGb", cbxTsnStateGb },
    { "cbxTsnStateHp", cbxTsnStateHp },
    { "cbxTsnStateLp", cbxTsnStateLp },
    CINT_ENTRY_LAST
};
#endif

static cint_enum_type_t __cint_fsal_cosq_enums[] =
{
    { "cbx_dp_t", __cint_enum_map__cbx_dp_t },
    { "cbx_schedule_mode_t", __cint_enum_map__cbx_schedule_mode_t },
    { "cbx_flowcontrol_t", __cint_enum_map__cbx_flowcontrol_t },
#ifdef CONFIG_TIMESYNC
    { "cbx_tsn_state_t", __cint_enum_map__cbx_tsn_state_t },
#endif
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_fsal_cosq_typedefs[] =
{
    CINT_ENTRY_LAST
};


static cint_constants_t __cint_fsal_cosq_constants[] =
{
   { "CBX_COSQ_INVALID", CBX_COSQ_INVALID },
   { "CBX_COSQ_MAX", CBX_COSQ_MAX },
   { "CBX_COS_COUNT", CBX_COS_COUNT },
   { "CBX_COSQ_SHAPER_MODE_AVB", CBX_COSQ_SHAPER_MODE_AVB },
   { "CBX_COSQ_SHAPER_MODE_PACKETS", CBX_COSQ_SHAPER_MODE_PACKETS },
#ifdef CONFIG_TIMESYNC
   { "CBX_COSQ_TSN_QUEUE_CONFIG_ENABLE", CBX_COSQ_TSN_QUEUE_CONFIG_ENABLE },
   { "CBX_COSQ_TSN_CT_CONFIG_ONE_SHOT", CBX_COSQ_TSN_CT_CONFIG_ONE_SHOT },
   { "CBX_COSQ_TSN_CONFIG_CHANGE", CBX_COSQ_TSN_CONFIG_CHANGE },
#endif
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_cosq_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_cosq_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_cosq_create),
        CINT_FWRAPPER_ENTRY(cbx_cosq_destroy),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_default_tc_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_default_tc_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_default_pcp_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_default_pcp_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_pcp2tc_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_pcp2tc_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_tc2pcp_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_tc2pcp_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_dscp2tc_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_dscp2tc_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_tc2dscp_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_tc2dscp_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_sched_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_sched_set),
        CINT_FWRAPPER_ENTRY(cbx_flowcontrol_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_cosq_flowcontrol_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_flowcontrol_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_flowcontrol_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_flowcontrol_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_pfc_flowcontrol_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_pfc_flowcontrol_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_shaper_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_port_shaper_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_queue_shaper_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_queue_shaper_set),
#ifdef CONFIG_TIMESYNC
        CINT_FWRAPPER_ENTRY(cbx_cosq_tsn_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_cosq_queue_tsn_config_set),
        CINT_FWRAPPER_ENTRY(cbx_cosq_queue_tsn_config_get),
#endif
        CINT_ENTRY_LAST
    };

cint_data_t fsal_cosq_cint_data =
    {
        NULL,
        __cint_fsal_cosq_functions,
        __cint_fsal_cosq_structures,
        __cint_fsal_cosq_enums,
        __cint_fsal_cosq_typedefs,
        __cint_fsal_cosq_constants,
        __cint_fsal_cosq_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */


