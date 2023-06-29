/*
 *
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * fsal_mcast_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_mcast_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <mcast.h>

CINT_FWRAPPER_CREATE_VP1(cbx_mcast_params_t_init,
                         cbx_mcast_params_t*, cbx_mcast_params_t,
                         mcast_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_mcast_create,
                         cbx_mcast_params_t *, cbx_mcast_params_t,
                         mcast_params, 1, 0,
                         cbx_mcastid_t *, cbx_mcastid_t, mcastid, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_mcast_destroy,
                         cbx_mcastid_t, cbx_mcastid_t, mcastid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_mcast_member_add,
                         cbx_mcastid_t, cbx_mcastid_t, mcastid, 0, 0,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_mcast_member_remove,
                         cbx_mcastid_t, cbx_mcastid_t, mcastid, 0, 0,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_mcast_member_info_get,
                         cbx_mcastid_t, cbx_mcastid_t, mcastid, 0, 0,
                         cbx_mcast_member_info_t *, cbx_mcast_member_info_t,
                         mcast_member_info, 1, 0);



static void*
__cint_maddr__cbx_mcast_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_mcast_params_t* s = (cbx_mcast_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->lilt_mode); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_mcast_member_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_mcast_member_info_t* s = (cbx_mcast_member_info_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->max_portid); break;
        case 1: rv = &(s->portid_array); break;
        case 2: rv = &(s->valid_portid); break;
        default: rv = NULL; break;
    }
    return rv;
}
static cint_parameter_desc_t __cint_struct_members__cbx_mcast_params_t[] =
{
    {
        "cbx_mcast_lilt_mode_t",
        "lilt_mode",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_mcast_member_info_t[] =
{
    {
        "int",
        "max_portid",
        0,
        0
    },
    {
        "cbx_portid_t",
        "portid_array",
        1,
        0
    },
    {
        "int",
        "valid_portid",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_struct_type_t __cint_fsal_mcast_structures[] =
{
    {
    "cbx_mcast_params_t",
    sizeof(cbx_mcast_params_t),
    __cint_struct_members__cbx_mcast_params_t,
    __cint_maddr__cbx_mcast_params_t
    },
    {
    "cbx_mcast_member_info_t",
    sizeof(cbx_mcast_member_info_t),
    __cint_struct_members__cbx_mcast_member_info_t,
    __cint_maddr__cbx_mcast_member_info_t
    },
    CINT_ENTRY_LAST
};



static cint_enum_map_t __cint_enum_map__cbx_mcast_lilt_mode_t[] =
{
    { "cbxMcastLiltModeNone", cbxMcastLiltModeNone },
    { "cbxMcastLiltModeArray", cbxMcastLiltModeArray },
    { "cbxMcastLiltModeList", cbxMcastLiltModeList },
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_fsal_mcast_enums[] =
{
    { "cbx_mcast_lilt_mode_t", __cint_enum_map__cbx_mcast_lilt_mode_t },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_fsal_mcast_typedefs[] =
{
    CINT_ENTRY_LAST
};


static cint_constants_t __cint_fsal_mcast_constants[] =
{
   { "CBX_MCAST_INVALID", CBX_MCAST_INVALID },
   { "CBX_MCAST_MAX", CBX_MCAST_MAX },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_mcast_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_mcast_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_mcast_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_mcast_create),
        CINT_FWRAPPER_ENTRY(cbx_mcast_destroy),
        CINT_FWRAPPER_ENTRY(cbx_mcast_member_add),
        CINT_FWRAPPER_ENTRY(cbx_mcast_member_remove),
        CINT_FWRAPPER_ENTRY(cbx_mcast_member_info_get),
        CINT_ENTRY_LAST
    };

cint_data_t fsal_mcast_cint_data =
    {
        NULL,
        __cint_fsal_mcast_functions,
        __cint_fsal_mcast_structures,
        __cint_fsal_mcast_enums,
        __cint_fsal_mcast_typedefs,
        __cint_fsal_mcast_constants,
        __cint_fsal_mcast_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */


