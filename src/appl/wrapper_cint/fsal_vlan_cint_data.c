/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * fsal_vlan_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_vlan_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <vlan.h>

CINT_FWRAPPER_CREATE_VP1(cbx_vlan_params_t_init,
                         cbx_vlan_params_t*, cbx_vlan_params_t,
						 vlan_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_vlan_create,
                         cbx_vlan_params_t*, cbx_vlan_params_t,
						 vlan_params, 1, 0,
                         cbx_vlanid_t*, cbx_vlanid_t, vlanid, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_vlan_destroy,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0);
CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                         cbx_vlan_destroy_all);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_vlan_port_add,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         uint32_t, uint32_t, mode, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_vlan_port_remove,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0);
CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         cbx_vlan_port_get,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0,
                         int, int, array_max, 0, 0,
                         cbx_portid_t*, cbx_portid_t, portid_array, 1, 0,
                         uint32_t*, uint32_t, mode_array, 1, 0,
                         int*, int, array_size, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_vlan_get,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0,
                         cbx_vlan_params_t*, cbx_vlan_params_t,
						 vlan_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_vlan_set,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0,
                         cbx_vlan_params_t, cbx_vlan_params_t,
						 vlan_params, 0, 0);
#ifdef CONFIG_VLAN_PVID
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_vlan_port_pvid_set,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0);
#endif

static void*
__cint_maddr__cbx_vlan_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_vlan_params_t* s = (cbx_vlan_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->vlan); break;
        case 2: rv = &(s->learn_mode); break;
        case 3: rv = &(s->stgid); break;
        case 4: rv = &(s->mcastid); break;
        case 5: rv = &(s->ucastid); break;
        case 6: rv = &(s->isolation); break;
        default: rv = NULL; break;
    }
    return rv;
}


static cint_parameter_desc_t __cint_struct_members__cbx_vlan_params_t[] =
{
    {
        "uint32",
        "flags",
        0,
        0
    },
    {
        "cbx_vlan_t",
        "vlan",
        0,
        0
    },
    {
        "cbx_vlan_learn_mode_t",
        "learn_mode",
        0,
        0
    },
    {
        "cbx_stgid_t",
        "stgid",
        0,
        0
    },
    {
        "cbx_mcastid_t",
        "mcastid",
        0,
        0
    },
    {
        "cbx_portid_t",
        "ucastid",
        0,
        0
    },
    {
        "int",
        "isolation",
        0,
        CBX_VLAN_ISOLATION_MAX
    },
    CINT_ENTRY_LAST
};

static cint_struct_type_t __cint_fsal_vlan_structures[] =
{
    {
    "cbx_vlan_params_t",
    sizeof(cbx_vlan_params_t),
    __cint_struct_members__cbx_vlan_params_t,
    __cint_maddr__cbx_vlan_params_t
    },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_vlan_learn_mode_t[] =
{
    { "cbxVlanLearnModeDisable", cbxVlanLearnModeDisable },
    { "cbxVlanLearnModeCopyHost", cbxVlanLearnModeCopyHost },
    { "cbxVlanLearnModeMarkPending", cbxVlanLearnModeMarkPending },
    { "cbxVlanLearnModeMarkReady", cbxVlanLearnModeMarkReady },
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_fsal_vlan_enums[] =
{
    { "cbx_vlan_learn_mode_t", __cint_enum_map__cbx_vlan_learn_mode_t },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_fsal_vlan_typedefs[] =
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
        "uint32",
        "cbx_mcastid_t",
        0,
        0
    },
    {
        "uint32_t",
        "cbx_portid_t",
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
        "cbx_stgid_t",
        0,
        0
    },
    {
        "uint16",
        "cbx_vlan_t",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_constants_t __cint_fsal_vlan_constants[] =
{
   { "CBX_VLAN_PORT_MODE_UNTAGGED", CBX_VLAN_PORT_MODE_UNTAGGED },
   { "CBX_VLAN_PORT_MODE_TAGGED", CBX_VLAN_PORT_MODE_TAGGED },
   { "CBX_VLAN_WRED_ENABLE", CBX_VLAN_WRED_ENABLE },
   { "CBX_VLAN_MCAST_DGT", CBX_VLAN_MCAST_DGT },
   { "CBX_VLAN_DROP_ULF", CBX_VLAN_DROP_ULF },
   { "CBX_VLAN_DROP_DLF", CBX_VLAN_DROP_DLF },
   { "CBX_VLAN_DROP_SLF", CBX_VLAN_DROP_SLF },
   { "CBX_VLAN_DROP_BCAST", CBX_VLAN_DROP_BCAST },
   { "CBX_VLAN_ALLOCATE_VSI", CBX_VLAN_ALLOCATE_VSI },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_vlan_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_vlan_functions[] =
    {

        CINT_FWRAPPER_ENTRY(cbx_vlan_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_vlan_create),
        CINT_FWRAPPER_ENTRY(cbx_vlan_destroy),
        CINT_FWRAPPER_ENTRY(cbx_vlan_destroy_all),
        CINT_FWRAPPER_ENTRY(cbx_vlan_port_add),
        CINT_FWRAPPER_ENTRY(cbx_vlan_port_remove),
        CINT_FWRAPPER_ENTRY(cbx_vlan_port_get),
        CINT_FWRAPPER_ENTRY(cbx_vlan_get),
        CINT_FWRAPPER_ENTRY(cbx_vlan_set),
#ifdef CONFIG_VLAN_PVID
        CINT_FWRAPPER_ENTRY(cbx_vlan_port_pvid_set),
#endif

        CINT_ENTRY_LAST
    };
cint_data_t fsal_vlan_cint_data =
    {
        NULL,
        __cint_fsal_vlan_functions,
        __cint_fsal_vlan_structures,
        __cint_fsal_vlan_enums,
        __cint_fsal_vlan_typedefs,
        __cint_fsal_vlan_constants,
        __cint_fsal_vlan_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */



