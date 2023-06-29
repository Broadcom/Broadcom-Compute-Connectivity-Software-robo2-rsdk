/*
 *
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * fsal_mirror_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_mirror_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <mirror.h>


CINT_FWRAPPER_CREATE_VP1(cbx_mirror_dest_t_init,
                         cbx_mirror_dest_t*, cbx_mirror_dest_t,
                         mirror_dest, 1, 0);
CINT_FWRAPPER_CREATE_VP1(cbx_mirror_params_t_init,
                         cbx_mirror_params_t*, cbx_mirror_params_t,
                         mirror_params, 1, 0);
CINT_FWRAPPER_CREATE_VP1(cbx_mirror_port_params_t_init,
                         cbx_mirror_port_params_t*, cbx_mirror_port_params_t,
                         mirror_port_params, 1, 0);
CINT_FWRAPPER_CREATE_VP1(cbx_mirror_vlan_params_t_init,
                         cbx_mirror_vlan_params_t*, cbx_mirror_vlan_params_t,
                         mirror_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_mirror_dest_create,
                         cbx_mirror_dest_t*, cbx_mirror_dest_t,
                         mirror_dest, 1, 0,
                         cbx_mirror_destid_t*, cbx_mirror_destid_t,
                         mirror_destid, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_mirror_dest_destroy,
                         cbx_mirror_destid_t, cbx_mirror_destid_t,
                         mirror_destid, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_mirror_create,
                         cbx_mirror_params_t *, cbx_mirror_params_t,
                         mirror_params, 1, 0,
                         cbx_mirror_destid_t, cbx_mirror_destid_t,
                         mirror_destid, 0, 0,
                         cbx_mirrorid_t *, cbx_mirrorid_t, mirrorid, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_mirror_destroy,
                         cbx_mirrorid_t, cbx_mirrorid_t, mirrorid, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_mirror_port_add,
                         cbx_mirrorid_t, cbx_mirrorid_t, mirrorid, 0, 0,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_mirror_port_params_t *, cbx_mirror_port_params_t,
                         mirror_port_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_mirror_port_remove,
                         cbx_mirrorid_t, cbx_mirrorid_t, mirrorid, 0, 0,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_mirror_vlan_add,
                         cbx_mirrorid_t, cbx_mirrorid_t, mirrorid, 0, 0,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0,
                         cbx_mirror_vlan_params_t *, cbx_mirror_vlan_params_t,
                         mirror_vlan_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_mirror_vlan_remove,
                         cbx_mirrorid_t, cbx_mirrorid_t, mirrorid, 0, 0,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0);



static void*
__cint_maddr__cbx_mirror_dest_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_mirror_dest_t* s = (cbx_mirror_dest_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->port); break;
        case 2: rv = &(s->src_mac); break;
        case 3: rv = &(s->dst_mac); break;
        case 4: rv = &(s->vlan_id); break;
        case 5: rv = &(s->encap); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_mirror_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_mirror_params_t* s = (cbx_mirror_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->mirror_tc); break;
        case 2: rv = &(s->mirror_dp); break;
        case 3: rv = &(s->sample_rate); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_mirror_port_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_mirror_port_params_t* s = (cbx_mirror_port_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->mode); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_mirror_vlan_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_mirror_vlan_params_t* s = (cbx_mirror_vlan_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->mode); break;
        default: rv = NULL; break;
    }
    return rv;
}
static cint_parameter_desc_t __cint_struct_members__cbx_mirror_dest_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "cbx_portid_t",
        "port",
        0,
        0
    },
    {
        "eth_mac_t",
        "src_mac",
        0,
        0
    },
    {
        "eth_mac_t",
        "dst_mac",
        0,
        0
    },
    {
        "cbx_vlan_t",
        "vlan_id",
        0,
        0
    },
    {
        "cbx_encap_info_t",
        "encap",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_mirror_params_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "int",
        "mirror_tc",
        0,
        0
    },
    {
        "int",
        "mirror_dp",
        0,
        0
    },
    {
        "uint32_t",
        "sample_rate",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_mirror_port_params_t[] =
{
    {
        "cbx_mirror_mode_t",
        "mode",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_mirror_vlan_params_t[] =
{
    {
        "cbx_mirror_mode_t",
        "mode",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_struct_type_t __cint_fsal_mirror_structures[] =
{
    {
    "cbx_mirror_dest_t",
    sizeof(cbx_mirror_dest_t),
    __cint_struct_members__cbx_mirror_dest_t,
    __cint_maddr__cbx_mirror_dest_t
    },
    {
    "cbx_mirror_params_t",
    sizeof(cbx_mirror_params_t),
    __cint_struct_members__cbx_mirror_params_t,
    __cint_maddr__cbx_mirror_params_t
    },
    {
    "cbx_mirror_port_params_t",
    sizeof(cbx_mirror_port_params_t),
    __cint_struct_members__cbx_mirror_port_params_t,
    __cint_maddr__cbx_mirror_port_params_t
    },
    {
    "cbx_mirror_vlan_params_t",
    sizeof(cbx_mirror_vlan_params_t),
    __cint_struct_members__cbx_mirror_vlan_params_t,
    __cint_maddr__cbx_mirror_vlan_params_t
    },
    CINT_ENTRY_LAST
};


static cint_enum_map_t __cint_enum_map__cbx_mirror_mode_t[] =
{
    { "cbxMirrorModeIngress", cbxMirrorModeIngress },
    { "cbxMirrorModeEgress", cbxMirrorModeEgress },
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_fsal_mirror_enums[] =
{
    { "cbx_mirror_mode_t", __cint_enum_map__cbx_mirror_mode_t },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_fsal_mirror_typedefs[] =
{
    {
        "uint32_t",
        "cbx_mirrorid_t",
        0,
        0
    },
    {
        "uint32_t",
        "cbx_mirror_destid_t",
        0,
        0
    },
    {
        "int",
        "cbx_encap_info_t",
        0,
        0
    },
    {
        "unsigned char",
        "uint8",
        0,
        0
    },
    {
        "uint8",
        "uint8_t",
        0,
        0
    },
    {
        "uint8_t",
        "eth_mac_t",
        0,
        6
    },
    CINT_ENTRY_LAST
};

static cint_constants_t __cint_fsal_mirror_constants[] =
{
   { "CBX_MIRROR_INVALID", CBX_MIRROR_INVALID },
   { "CBX_MIRROR_MAX", CBX_MIRROR_MAX },
   { "CBX_MIRROR_DEST_INVALID", CBX_MIRROR_DEST_INVALID },
   { "CBX_MIRROR_DEST_MAX", CBX_MIRROR_DEST_MAX },
   { "CBX_MIRROR_DEST_ENCAP", CBX_MIRROR_DEST_ENCAP },
   { "CBX_MIRROR_TRUNCATE", CBX_MIRROR_TRUNCATE },
   { "CBX_MIRROR_MODE_PORT_INGRESS", CBX_MIRROR_MODE_PORT_INGRESS },
   { "CBX_MIRROR_MODE_PORT_EGRESS", CBX_MIRROR_MODE_PORT_EGRESS },
   { "CBX_MIRROR_MODE_VLAN_INGRESS", CBX_MIRROR_MODE_VLAN_INGRESS },
   { "CBX_MIRROR_MODE_VLAN_EGRESS", CBX_MIRROR_MODE_VLAN_EGRESS },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_mirror_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_mirror_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_mirror_dest_t_init),
        CINT_FWRAPPER_ENTRY(cbx_mirror_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_mirror_port_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_mirror_vlan_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_mirror_dest_create),
        CINT_FWRAPPER_ENTRY(cbx_mirror_dest_destroy),
        CINT_FWRAPPER_ENTRY(cbx_mirror_create),
        CINT_FWRAPPER_ENTRY(cbx_mirror_destroy),
        CINT_FWRAPPER_ENTRY(cbx_mirror_port_add),
        CINT_FWRAPPER_ENTRY(cbx_mirror_port_remove),
        CINT_FWRAPPER_ENTRY(cbx_mirror_vlan_add),
        CINT_FWRAPPER_ENTRY(cbx_mirror_vlan_remove),
        CINT_ENTRY_LAST
    };

cint_data_t fsal_mirror_cint_data =
    {
        NULL,
        __cint_fsal_mirror_functions,
        __cint_fsal_mirror_structures,
        __cint_fsal_mirror_enums,
        __cint_fsal_mirror_typedefs,
        __cint_fsal_mirror_constants,
        __cint_fsal_mirror_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */


