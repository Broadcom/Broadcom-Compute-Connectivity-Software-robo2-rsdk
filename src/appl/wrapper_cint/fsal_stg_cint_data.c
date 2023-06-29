/*
 *
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * fsal_stg_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_stg_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <stg.h>


CINT_FWRAPPER_CREATE_VP1(cbx_stg_params_t_init,
                         cbx_stg_params_t*, cbx_stg_params_t, stg_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_stg_create,
                         cbx_stg_params_t*, cbx_stg_params_t, stg_params, 1, 0,
                         cbx_stgid_t*, cbx_stgid_t, stgid, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_stg_destroy,
                         cbx_stgid_t, cbx_stgid_t, stgid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_stg_vlan_add,
                         cbx_stgid_t, cbx_stgid_t, stgid, 0, 0,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_stg_vlan_remove,
                         cbx_stgid_t, cbx_stgid_t, stgid, 0, 0,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_stg_vlan_remove_all,
                         cbx_stgid_t, cbx_stgid_t, stgid, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_stg_stp_set,
                         cbx_stgid_t, cbx_stgid_t, stgid, 0, 0,
                         cbx_portid_t, cbx_portid_t, port, 0, 0,
                         cbx_stg_stp_state_t, cbx_stg_stp_state_t,
						 stp_state, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_stg_stp_get,
                         cbx_stgid_t, cbx_stgid_t, stgid, 0, 0,
                         cbx_portid_t, cbx_portid_t, port, 0, 0,
                         cbx_stg_stp_state_t*, cbx_stg_stp_state_t,
						 stp_state, 1, 0);



static void*
__cint_maddr__cbx_stg_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_stg_params_t* s = (cbx_stg_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->stg_mode); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_stg_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_stg_info_t* s = (cbx_stg_info_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->max_portid); break;
        case 1: rv = &(s->portid_array); break;
        case 2: rv = &(s->valid_portid); break;
        default: rv = NULL; break;
    }
    return rv;
}



static cint_parameter_desc_t __cint_struct_members__cbx_stg_params_t[] =
{
    {
        "cbx_stg_mode_t",
        "stg_mode",
        0,
        0
    },

    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_stg_info_t[] =
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


static cint_struct_type_t __cint_fsal_stg_structures[] =
{
    {
    "cbx_stg_params_t",
    sizeof(cbx_stg_params_t),
    __cint_struct_members__cbx_stg_params_t,
    __cint_maddr__cbx_stg_params_t
    },
    {
    "cbx_stg_info_t",
    sizeof(cbx_stg_info_t),
    __cint_struct_members__cbx_stg_info_t,
    __cint_maddr__cbx_stg_info_t
    },
    CINT_ENTRY_LAST
};


static cint_enum_map_t __cint_enum_map__cbx_stg_mode_t[] =
{
    { "cbxStgModeGlobal", cbxStgModeGlobal },
    { "cbxStgModeNormal", cbxStgModeNormal },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__cbx_stg_stp_state_t[] =
{
    { "cbxStgStpStateDiscard", cbxStgStpStateDiscard },
    { "cbxStgStpStateLearn", cbxStgStpStateLearn },
    { "cbxStgStpStateForward", cbxStgStpStateForward },
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_fsal_stg_enums[] =
{
    { "cbx_stg_mode_t", __cint_enum_map__cbx_stg_mode_t },
    { "cbx_stg_stp_state_t", __cint_enum_map__cbx_stg_stp_state_t },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_fsal_stg_typedefs[] =
{
    {
#ifdef CONFIG_EXTERNAL_HOST
        "unsigned int",
#else
        "unsigned long",
#endif
        "uint32",
        0,
        0
    },
    {
        "uint32",
        "uint32_t",
        0,
        0
    },
    {
        "uint32_t",
        "cbx_stgid_t",
        0,
        0
    },
    {
        "uint32_t",
        "cbx_vlanid_t",
        0,
        0
    },
    {
        "uint32_t",
        "cbx_portid_t",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_constants_t __cint_fsal_stg_constants[] =
{
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_stg_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_stg_functions[] =
    {


        CINT_FWRAPPER_ENTRY(cbx_stg_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_stg_create),
        CINT_FWRAPPER_ENTRY(cbx_stg_destroy),
        CINT_FWRAPPER_ENTRY(cbx_stg_vlan_add),
        CINT_FWRAPPER_ENTRY(cbx_stg_vlan_remove),
        CINT_FWRAPPER_ENTRY(cbx_stg_vlan_remove_all),
        CINT_FWRAPPER_ENTRY(cbx_stg_stp_set),
        CINT_FWRAPPER_ENTRY(cbx_stg_stp_get),

        CINT_ENTRY_LAST
    };
cint_data_t fsal_stg_cint_data =
    {
        NULL,
        __cint_fsal_stg_functions,
        __cint_fsal_stg_structures,
        __cint_fsal_stg_enums,
        __cint_fsal_stg_typedefs,
        __cint_fsal_stg_constants,
        __cint_fsal_stg_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */

