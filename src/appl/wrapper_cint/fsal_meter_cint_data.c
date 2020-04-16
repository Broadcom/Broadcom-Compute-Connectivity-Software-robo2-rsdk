/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * fsal_meter_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_meter_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <cosq.h>
#include <meter.h>

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_meter_profile_create,
                         cbx_meter_profile_params_t*, cbx_meter_profile_params_t,
                         meter_profile_params, 1, 0,
                         cbx_meter_profile_t*, cbx_meter_profile_t,
                         profileid, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_meter_profile_destroy,
                         cbx_meter_profile_t, cbx_meter_profile_t,
                         profileid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_meter_profile_get,
                         cbx_meter_profile_t, cbx_meter_profile_t,
                         profileid, 0, 0,
                         cbx_meter_profile_params_t*, cbx_meter_profile_params_t,
                         meter_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_meter_profile_set,
                         cbx_meter_profile_t, cbx_meter_profile_t,
                         profileid, 0, 0,
                         cbx_meter_profile_params_t*, cbx_meter_profile_params_t,
                         meter_params, 1, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_meter_create,
                         cbx_meter_profile_t, cbx_meter_profile_t,
                         profileid, 0, 0,
                         cbx_meter_params_t *, cbx_meter_params_t,
                         meter_params, 1, 0,
                         cbx_meterid_t *, cbx_meterid_t, meterid, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_meter_destroy,
                         cbx_meterid_t, cbx_meterid_t, meterid, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_meter_pm_enable,
                         cbx_meterid_t, cbx_meterid_t, meterid, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_meter_pm_disable,
                         cbx_meterid_t, cbx_meterid_t, meterid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_meter_pm_get,
                         cbx_meterid_t, cbx_meterid_t, meterid, 0, 0,
                         cbx_meter_pm_params_t*, cbx_meter_pm_params_t,
                         pm_params, 1, 0);



static void*
__cint_maddr__cbx_meter_profile_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_meter_profile_params_t* s = (cbx_meter_profile_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->type); break;
        case 2: rv = &(s->mode); break;
        case 3: rv = &(s->ckbits_sec); break;
        case 4: rv = &(s->ckbits_burst); break;
        case 5: rv = &(s->pkbits_sec); break;
        case 6: rv = &(s->pkbits_burst); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_meter_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_meter_params_t* s = (cbx_meter_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->portid); break;
        case 1: rv = &(s->traffic_type); break;
        case 2: rv = &(s->tc); break;
        case 3: rv = &(s->trap_group_index); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_meter_pm_counter_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_meter_pm_counter_t* s = (cbx_meter_pm_counter_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->packet); break;
        case 1: rv = &(s->byte); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_meter_pm_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_meter_pm_params_t* s = (cbx_meter_pm_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->cbx_meter_pm_green); break;
        case 1: rv = &(s->cbx_meter_pm_yellow); break;
        case 2: rv = &(s->cbx_meter_pm_red); break;
        case 3: rv = &(s->cbx_meter_pm_reserved); break;
        case 4: rv = &(s->cbx_meter_pm_g2y); break;
        case 5: rv = &(s->cbx_meter_pm_y2r); break;
        case 6: rv = &(s->cbx_meter_pm_g2r); break;
        case 7: rv = &(s->cbx_meter_pm_y2g); break;
        case 8: rv = &(s->cbx_meter_pm_r2g); break;
        case 9: rv = &(s->cbx_meter_pm_r2y); break;
        case 10: rv = &(s->cbx_meter_pm_g2d); break;
        case 11: rv = &(s->cbx_meter_pm_y2d); break;
        case 12: rv = &(s->cbx_meter_pm_r2d); break;
        default: rv = NULL; break;
    }
    return rv;
}


static cint_parameter_desc_t __cint_struct_members__cbx_meter_profile_params_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "cbx_meter_type_t",
        "type",
        0,
        0
    },
    {
        "cbx_meter_mode_t",
        "mode",
        0,
        0
    },
    {
        "uint32_t",
        "ckbits_sec",
        0,
        0
    },
    {
        "uint32_t",
        "ckbits_burst",
        0,
        0
    },
    {
        "uint32_t",
        "pkbits_sec",
        0,
        0
    },
    {
        "uint32_t",
        "pkbits_burst",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_struct_members__cbx_meter_params_t[] =
{
    {
        "cbx_portid_t",
        "portid",
        0,
        0
    },
    {
        "cbx_traffic_type_t",
        "traffic_type",
        0,
        0
    },
    {
        "cbx_tc_t",
        "tc",
        0,
        0
    },
    {
        "int",
        "trap_group_index",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_meter_pm_counter_t[] =
{
    {
        "uint64_t",
        "packet",
        0,
        0
    },
    {
        "uint64_t",
        "byte",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_meter_pm_params_t[] =
{
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_green",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_yellow",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_red",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_reserved",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_g2y",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_y2r",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_g2r",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_y2g",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_r2g",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_r2y",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_g2d",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_y2d",
        0,
        0
    },
    {
        "cbx_meter_pm_counter_t",
        "cbx_meter_pm_r2d",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_struct_type_t __cint_fsal_meter_structures[] =
{
    {
    "cbx_meter_profile_params_t",
    sizeof(cbx_meter_profile_params_t),
    __cint_struct_members__cbx_meter_profile_params_t,
    __cint_maddr__cbx_meter_profile_params_t
    },
    {
    "cbx_meter_params_t",
    sizeof(cbx_meter_params_t),
    __cint_struct_members__cbx_meter_params_t,
    __cint_maddr__cbx_meter_params_t
    },
    {
    "cbx_meter_pm_counter_t",
    sizeof(cbx_meter_pm_counter_t),
    __cint_struct_members__cbx_meter_pm_counter_t,
    __cint_maddr__cbx_meter_pm_counter_t
    },
    {
    "cbx_meter_pm_params_t",
    sizeof(cbx_meter_pm_params_t),
    __cint_struct_members__cbx_meter_pm_params_t,
    __cint_maddr__cbx_meter_pm_params_t
    },
    CINT_ENTRY_LAST
};


static cint_enum_map_t __cint_enum_map__cbx_meter_type_t[] =
{
    { "cbxMeterBufferManager", cbxMeterBufferManager },
    { "cbxMeterTrafficType", cbxMeterTrafficType },
    { "cbxMeterTrapGroup", cbxMeterTrapGroup },
    { "cbxMeterCFP", cbxMeterCFP },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__cbx_meter_mode_t[] =
{
    { "cbxMeterModeSrTCM", cbxMeterModeSrTCM },
    { "cbxMeterModeTrTCM", cbxMeterModeTrTCM },
    { "cbxMeterModeMEF", cbxMeterModeMEF },
    { "cbxMeterModeMEFBw", cbxMeterModeMEFBw },
    { "cbxMeterModeCoupledMEFBw", cbxMeterModeCoupledMEFBw },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__cbx_traffic_type_t[] =
{
    { "cbxTrafficTypeUnicast", cbxTrafficTypeUnicast },
    { "cbxTrafficTypeMulticast", cbxTrafficTypeMulticast },
    { "cbxTrafficTypeBroadcast", cbxTrafficTypeBroadcast },
    { "cbxTrafficTypeDLF", cbxTrafficTypeDLF },
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


static cint_enum_type_t __cint_fsal_meter_enums[] =
{
    { "cbx_meter_type_t", __cint_enum_map__cbx_meter_type_t },
    { "cbx_meter_mode_t", __cint_enum_map__cbx_meter_mode_t },
    { "cbx_traffic_type_t", __cint_enum_map__cbx_traffic_type_t },
    { "cbx_dp_t", __cint_enum_map__cbx_dp_t },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_fsal_meter_typedefs[] =
{
    {
        "uint32_t",
        "cbx_meter_profile_t",
        0,
        0
    },
    {
        "uint32_t",
        "cbx_meterid_t",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_constants_t __cint_fsal_meter_constants[] =
{
   { "CBX_METER_INVALID", CBX_METER_INVALID },
   { "CBX_METER_MAX", CBX_METER_MAX },
   { "CBX_METER_TYPE_NORMAL", CBX_METER_TYPE_NORMAL },
   { "CBX_METER_COLOR_BLIND", CBX_METER_COLOR_BLIND },
   { "CBX_METER_DROP_RED", CBX_METER_DROP_RED },
   { "CBX_METER_SHAPING", CBX_METER_SHAPING },
   { "CBX_METER_MODE_PACKETS", CBX_METER_MODE_PACKETS },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_meter_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_meter_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_meter_profile_create),
        CINT_FWRAPPER_ENTRY(cbx_meter_profile_destroy),
        CINT_FWRAPPER_ENTRY(cbx_meter_profile_get),
        CINT_FWRAPPER_ENTRY(cbx_meter_profile_set),
        CINT_FWRAPPER_ENTRY(cbx_meter_create),
        CINT_FWRAPPER_ENTRY(cbx_meter_destroy),
        CINT_FWRAPPER_ENTRY(cbx_meter_pm_enable),
        CINT_FWRAPPER_ENTRY(cbx_meter_pm_disable),
        CINT_FWRAPPER_ENTRY(cbx_meter_pm_get),
        CINT_ENTRY_LAST
    };

cint_data_t fsal_meter_cint_data =
    {
        NULL,
        __cint_fsal_meter_functions,
        __cint_fsal_meter_structures,
        __cint_fsal_meter_enums,
        __cint_fsal_meter_typedefs,
        __cint_fsal_meter_constants,
        __cint_fsal_meter_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */


