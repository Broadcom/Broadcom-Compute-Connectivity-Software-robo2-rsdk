/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * fsal_l2_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_l2_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <l2.h>

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_l2_entry_add,
                         cbx_l2_entry_t *, cbx_l2_entry_t, l2entry, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_l2_addr_remove,
                         cbx_l2_entry_t *, cbx_l2_entry_t, l2entry, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_l2_lookup,
                         cbx_l2_entry_t *, cbx_l2_entry_t, l2entry, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_l2_lookup_next,
                         cbx_l2_entry_t *, cbx_l2_entry_t, l2entry, 1, 0,
                         cbx_l2_entry_t *, cbx_l2_entry_t, l2entry_next, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_l2_age_timer_set,
                         int, int, age_seconds, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_l2_age_timer_get,
                         int*, int, age_seconds, 1, 0);
CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                         cbx_l2_reseed);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_l2_flush,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_l2_port_flush,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0);



static void*
__cint_maddr__cbx_l2_param_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_l2_param_t* s = (cbx_l2_param_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->dest_type); break;
        case 2: rv = &(s->portid); break;
        case 3: rv = &(s->upd_ctrl); break;
        case 4: rv = &(s->fwd_ctrl); break;
        case 5: rv = &(s->e_cid); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_l2_entry_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_l2_entry_t* s = (cbx_l2_entry_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->src_mac); break;
        case 1: rv = &(s->vlanid); break;
        case 2: rv = &(s->params); break;
        default: rv = NULL; break;
    }
    return rv;
}
static cint_parameter_desc_t __cint_struct_members__cbx_l2_param_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "cbx_l2_addr_dest_type_t",
        "dest_type",
        0,
        0
    },
    {
        "cbx_portid_t",
        "portid",
        0,
        0
    },
    {
        "cbx_l2_addr_movement_t",
        "upd_ctrl",
        0,
        0
    },
    {
        "cbx_l2_addr_ctrl_t",
        "fwd_ctrl",
        0,
        0
    },
    {
        "uint32_t",
        "e_cid",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_l2_entry_t[] =
{
    {
        "eth_mac_t",
        "src_mac",
        0,
        0
    },
    {
        "cbx_vlanid_t",
        "vlanid",
        0,
        0
    },
    {
        "cbx_l2_param_t",
        "params",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_struct_type_t __cint_fsal_l2_structures[] =
{
    {
    "cbx_l2_param_t",
    sizeof(cbx_l2_param_t),
    __cint_struct_members__cbx_l2_param_t,
    __cint_maddr__cbx_l2_param_t
    },
    {
    "cbx_l2_entry_t",
    sizeof(cbx_l2_entry_t),
    __cint_struct_members__cbx_l2_entry_t,
    __cint_maddr__cbx_l2_entry_t
    },
    CINT_ENTRY_LAST
};



static cint_enum_map_t __cint_enum_map__cbx_l2_addr_ctrl_t[] =
{
    { "cbxL2AddrCtrlNormal", cbxL2AddrCtrlNormal },
    { "cbxL2AddrCtrlSrcDrop", cbxL2AddrCtrlSrcDrop },
    { "cbxL2AddrCtrlCopyCPU", cbxL2AddrCtrlCopyCPU },
    { "cbxL2AddrCtrlSrcDropCopyCPU", cbxL2AddrCtrlSrcDropCopyCPU },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__cbx_l2_addr_movement_t[] =
{
    { "cbxL2AddrMovementUpdate", cbxL2AddrMovementUpdate },
    { "cbxL2AddrMovementUpdateCopy", cbxL2AddrMovementUpdateCopy },
    { "cbxL2AddrMovementCopy", cbxL2AddrMovementCopy },
    { "cbxL2AddrMovementDisable", cbxL2AddrMovementDisable },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__cbx_l2_addr_dest_type_t[] =
{
    { "cbxL2AddrDestDrop", cbxL2AddrDestDrop },
    { "cbxL2AddrDestMcast", cbxL2AddrDestMcast },
    { "cbxL2AddrDestDLI", cbxL2AddrDestDLI },
    { "cbxL2AddrDestDPG", cbxL2AddrDestDPG },
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_fsal_l2_enums[] =
{
    { "cbx_l2_addr_ctrl_t", __cint_enum_map__cbx_l2_addr_ctrl_t },
    { "cbx_l2_addr_movement_t", __cint_enum_map__cbx_l2_addr_movement_t },
    { "cbx_l2_addr_dest_type_t", __cint_enum_map__cbx_l2_addr_dest_type_t },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_fsal_l2_typedefs[] =
{
    CINT_ENTRY_LAST
};


static cint_constants_t __cint_fsal_l2_constants[] =
{
   { "CBX_L2_INVALID", CBX_L2_INVALID },
   { "CBX_L2_MAX", CBX_L2_MAX },
   { "CBX_L2_ADDR_STATIC", CBX_L2_ADDR_STATIC },
   { "CBX_L2_ADDR_ECID", CBX_L2_ADDR_ECID },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_l2_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_l2_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_l2_entry_add),
        CINT_FWRAPPER_ENTRY(cbx_l2_addr_remove),
        CINT_FWRAPPER_ENTRY(cbx_l2_lookup),
        CINT_FWRAPPER_ENTRY(cbx_l2_lookup_next),
        CINT_FWRAPPER_ENTRY(cbx_l2_age_timer_set),
        CINT_FWRAPPER_ENTRY(cbx_l2_age_timer_get),
        CINT_FWRAPPER_ENTRY(cbx_l2_reseed),
        CINT_FWRAPPER_ENTRY(cbx_l2_flush),
        CINT_FWRAPPER_ENTRY(cbx_l2_port_flush),
        CINT_ENTRY_LAST
    };

cint_data_t fsal_l2_cint_data =
    {
        NULL,
        __cint_fsal_l2_functions,
        __cint_fsal_l2_structures,
        __cint_fsal_l2_enums,
        __cint_fsal_l2_typedefs,
        __cint_fsal_l2_constants,
        __cint_fsal_l2_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */


