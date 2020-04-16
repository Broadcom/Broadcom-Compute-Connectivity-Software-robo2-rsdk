/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * fsal_lif_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <lif.h>

CINT_FWRAPPER_CREATE_VP1(cbx_vlan_lif_params_t_init,
                         cbx_vlan_lif_params_t*, cbx_vlan_lif_params_t,
						 vlan_lif_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_vlan_lif_create,
                         cbx_vlan_lif_params_t*, cbx_vlan_lif_params_t,
						 vlan_lif_params, 1, 0,
                         cbx_portid_t*, cbx_portid_t, vlan_lifid, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_vlan_lif_destroy,
                         cbx_portid_t, cbx_portid_t, vlan_lifid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_vlan_lif_find,
                         cbx_vlan_lif_params_t*, cbx_vlan_lif_params_t,
						 vlan_lif_params, 1, 0,
                         cbx_portid_t*, cbx_portid_t, vlan_lifid, 1, 0);


static void*
__cint_maddr__cbx_vlan_lif_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_vlan_lif_params_t* s = (cbx_vlan_lif_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->criteria); break;
        case 2: rv = &(s->default_tc); break;
        case 3: rv = &(s->default_dp); break;
        case 4: rv = &(s->portid); break;
        case 5: rv = &(s->match_vlan); break;
        case 6: rv = &(s->match_inner_vlan); break;
        case 7: rv = &(s->vlan_action); break;
        case 8: rv = &(s->egress_vlan); break;
        case 9: rv = &(s->inner_vlan_action); break;
        case 10: rv = &(s->egress_inner_vlan); break;
        default: rv = NULL; break;
    }
    return rv;
}


static cint_parameter_desc_t __cint_struct_members__cbx_vlan_lif_params_t[] =
{
    {
        "uint32",
        "flags",
        0,
        0
    },
    {
        "cbx_vlan_lif_match_t",
        "criteria",
        0,
        0
    },
    {
        "int",
        "default_tc",
        0,
        0
    },
    {
        "int",
        "default_dp",
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
        "cbx_vlan_t",
        "match_vlan",
        0,
        0
    },
    {
        "cbx_vlan_t",
        "match_inner_vlan",
        0,
        0
    },
    {
        "cbx_vlan_lif_tag_actions_t",
        "vlan_action",
        0,
        0
    },
    {
        "cbx_vlan_t",
        "egress_vlan",
        0,
        0
    },
    {
        "cbx_vlan_lif_tag_actions_t",
        "inner_vlan_action",
        0,
        0
    },
    {
        "cbx_vlan_t",
        "egress_inner_vlan",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_struct_type_t __cint_fsal_lif_structures[] =
{
    {
    "cbx_vlan_lif_params_t",
    sizeof(cbx_vlan_lif_params_t),
    __cint_struct_members__cbx_vlan_lif_params_t,
    __cint_maddr__cbx_vlan_lif_params_t
    },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_vlan_lif_tag_actions_t[] =
{
    { "cbxVlanLifTagActionNone", cbxVlanLifTagActionNone },
    { "cbxVlanLifTagActionAdd", cbxVlanLifTagActionAdd },
    { "cbxVlanLifTagActionReplace", cbxVlanLifTagActionReplace },
    { "cbxVlanLifTagActionDelete", cbxVlanLifTagActionDelete },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_vlan_lif_match_t[] =
{
    { "cbxVlanLifMatchInvalid", cbxVlanLifMatchInvalid},
    { "cbxVlanLifMatchNone", cbxVlanLifMatchNone},
    { "cbxVlanLifMatchPortVlan", cbxVlanLifMatchPortVlan},
    { "cbxVlanLifMatchPortVlanStacked", cbxVlanLifMatchPortVlanStacked},
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_fsal_lif_enums[] =
{
    { "cbx_vlan_lif_tag_actions_t", __cint_enum_map__cbx_vlan_lif_tag_actions_t },
    { "cbx_vlan_lif_match_t", __cint_enum_map__cbx_vlan_lif_match_t },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_fsal_lif_typedefs[] =
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
    CINT_ENTRY_LAST
};

static cint_constants_t __cint_fsal_lif_constants[] =
{
   { "CBX_VLAN_LIF_INVALID", CBX_VLAN_LIF_INVALID },
   { "CBX_VLAN_LIF_MAX", CBX_VLAN_LIF_MAX },
   { "CBX_VLAN_LIF_INNER_VLAN_PRESERVE", CBX_VLAN_LIF_INNER_VLAN_PRESERVE },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_lif_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_lif_functions[] =
    {

        CINT_FWRAPPER_ENTRY(cbx_vlan_lif_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_vlan_lif_create),
        CINT_FWRAPPER_ENTRY(cbx_vlan_lif_destroy),
        CINT_FWRAPPER_ENTRY(cbx_vlan_lif_find),
        CINT_ENTRY_LAST
    };
cint_data_t fsal_lif_cint_data =
    {
        NULL,
        __cint_fsal_lif_functions,
        __cint_fsal_lif_structures,
        __cint_fsal_lif_enums,
        __cint_fsal_lif_typedefs,
        __cint_fsal_lif_constants,
        __cint_fsal_lif_function_pointers
    };
#endif /* INCLUDE_LIB_CINT */
