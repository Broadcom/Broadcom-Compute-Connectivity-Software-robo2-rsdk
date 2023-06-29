/*
 *
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * fsal_port_cint_data.c
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
#include <fsal/port.h>
#include <fsal_int/port.h>
#include <fsal_int/trap.h>

CINT_FWRAPPER_CREATE_VP1(cbx_port_params_t_init,
                         cbx_port_params_t*, cbx_port_params_t,
						 port_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_port_create,
                         cbx_port_params_t*, cbx_port_params_t,
						 port_params, 1, 0,
                         cbx_portid_t*, cbx_portid_t, portid, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_port_destroy,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_port_info_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_port_params_t*, cbx_port_params_t,
						 port_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_port_info_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_port_params_t*, cbx_port_params_t,
						 port_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_port_lookup,
                         cbx_port_lookup_t*, cbx_port_lookup_t, lookup, 1, 0,
                         cbx_portid_t*, cbx_portid_t, port_id, 1, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_port_isolation_get,
                         cbx_port_t, cbx_port_t, port, 0, 0,
                         cbx_port_isolation_t*, cbx_port_isolation_t,
						 port_iso, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_port_isolation_set,
                         cbx_port_t, cbx_port_t, port, 0, 0,
                         cbx_port_isolation_t*, cbx_port_isolation_t,
						 port_iso, 1, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_port_attribute_get,
                         cbx_port_t, cbx_port_t, port, 0, 0,
                         cbx_port_attr_t, cbx_port_attr_t, attr, 0, 0,
                         uint32_t*, uint32_t, value, 1, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_port_attribute_set,
                         cbx_port_t, cbx_port_t, port, 0, 0,
                         cbx_port_attr_t, cbx_port_attr_t, attr, 0, 0,
                         uint32_t, uint32_t, value, 0, 0);
CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         cbx_port_phy_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         uint32_t, uint32_t, flags, 0, 0,
                         uint32_t, uint32_t, phy_reg_addr, 0, 0,
                         uint32_t*, uint32_t, phy_data, 1, 0);
CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         cbx_port_phy_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         uint32_t, uint32_t, flags, 0, 0,
                         uint32_t, uint32_t, phy_reg_addr, 0, 0,
                         uint32_t, uint32_t, phy_data, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbxi_port_loopback_set,
                         int, int, unit, 0, 0,
                         cbx_port_t, cbx_port_t, port, 0, 0,
                         int, int, loopback, 0, 0);
CINT_FWRAPPER_CREATE_VP0(cbxi_port_phy_info);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbxi_trap_dest_update,
                         int, int, unit, 0, 0,
                         int, int, trap_dest_port, 0, 0);
#ifdef CONFIG_CASCADED_MODE
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbxi_cascade_port_delete,
                         int, int, portid, 0, 0);
#endif
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbxi_port_display_eye_scan,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0);

static void*
__cint_maddr__cbx_port_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_port_params_t* s = (cbx_port_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->port_type); break;
        case 2: rv = &(s->port_index); break;
        case 3: rv = &(s->sli_mode); break;
        case 4: rv = &(s->ac_policy); break;
        case 5: rv = &(s->default_vid); break;
        case 6: rv = &(s->default_tc); break;
        case 7: rv = &(s->default_dp); break;
        case 8: rv = &(s->learn_limit); break;
        case 9: rv = &(s->mtu); break;
        case 10: rv = &(s->port_group); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_port_isolation_t(void* p, int mnum,
                                    cint_struct_type_t* parent)
{
    void* rv;
    cbx_port_isolation_t* s = (cbx_port_isolation_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->group); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_port_lookup_t(void* p, int mnum,
                                    cint_struct_type_t* parent)
{
    void* rv;
    cbx_port_lookup_t* s = (cbx_port_lookup_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->port_index); break;
        default: rv = NULL; break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__cbx_port_params_t[] =
{
    {
        "uint32",
        "flags",
        0,
        0
    },
    {
        "cbx_port_type_t",
        "port_type",
        0,
        0
    },
    {
        "int",
        "port_index",
        0,
        0
    },
    {
        "cbx_port_sli_mode_t",
        "sli_mode",
        0,
        0
    },
    {
        "cbx_cosq_threshold_t",
        "ac_policy",
        0,
        0
    },
    {
        "cbx_vlan_t",
        "default_vid",
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
        "int",
        "learn_limit",
        0,
        0
    },
    {
        "int",
        "mtu",
        0,
        0
    },
    {
        "int",
        "port_group",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_port_isolation_t[] =
{
    {
        "cbx_port_isolation_group_t",
        "group",
        0,
        0
    },

    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_port_lookup_t[] =
{
    {
        "int",
        "port_index",
        0,
        0
    },

    CINT_ENTRY_LAST
};


static cint_struct_type_t __cint_fsal_port_structures[] =
{
    {
    "cbx_port_params_t",
    sizeof(cbx_port_params_t),
    __cint_struct_members__cbx_port_params_t,
    __cint_maddr__cbx_port_params_t
    },
    {
    "cbx_port_isolation_t",
    sizeof(cbx_port_isolation_t),
    __cint_struct_members__cbx_port_isolation_t,
    __cint_maddr__cbx_port_isolation_t
    },
    {
    "cbx_port_lookup_t",
    sizeof(cbx_port_lookup_t),
    __cint_struct_members__cbx_port_lookup_t,
    __cint_maddr__cbx_port_lookup_t
    },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_port_type_t[] =
{
    { "cbxPortTypeCustomer", cbxPortTypeCustomer },
    { "cbxPortTypeProvider", cbxPortTypeProvider },
    { "cbxPortTypeHost", cbxPortTypeHost },
    { "cbxPortTypeCascade", cbxPortTypeCascade},
    { "cbxPortTypeExtender", cbxPortTypeExtender },
    { "cbxPortTypeExtenderCascade", cbxPortTypeExtenderCascade },
    { "cbxPortTypeExtenderUpstream", cbxPortTypeExtenderUpstream },
    { "cbxPortTypePortVLAN", cbxPortTypePortVLAN },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__cbx_port_sli_mode_t[] =
{
    { "cbxPortSliPvCtag", cbxPortSliPvCtag },
    { "cbxPortSliPvStag", cbxPortSliPvStag },
    { "cbxPortSliPvNovid", cbxPortSliPvNovid },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__cbx_cosq_threshold_t[] =
{
    { "cbxPortAdmissionControlPolicy0", cbxPortAdmissionControlPolicy0 },
    { "cbxPortAdmissionControlPolicy1", cbxPortAdmissionControlPolicy1 },
    { "cbxPortAdmissionControlPolicy2", cbxPortAdmissionControlPolicy2 },
    { "cbxPortAdmissionControlPolicy3", cbxPortAdmissionControlPolicy3 },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__cbx_port_isolation_group_t[] =
{
    { "cbxPortIsolation_0", cbxPortIsolation_0 },
    { "cbxPortIsolation_1", cbxPortIsolation_1 },
    { "cbxPortIsolation_2", cbxPortIsolation_2 },
    { "cbxPortIsolation_3", cbxPortIsolation_3 },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__cbx_port_attr_t[] =
{
    { "cbxPortAttrDuplex", cbxPortAttrDuplex },
    { "cbxPortAttrSpeed", cbxPortAttrSpeed },
    { "cbxPortAttrMTU", cbxPortAttrMTU },
    { "cbxPortAttrLinkStatus", cbxPortAttrLinkStatus },
    { "cbxPortAttrAutoNegotiate", cbxPortAttrAutoNegotiate },
    { "cbxPortAttrAdmin", cbxPortAttrAdmin },
    { "cbxPortPhyEEE", cbxPortPhyEEE },
    { "cbxPortPhyMediaType", cbxPortPhyMediaType },
    CINT_ENTRY_LAST
};


static cint_enum_type_t __cint_fsal_port_enums[] =
{
    { "cbx_port_type_t", __cint_enum_map__cbx_port_type_t },
    { "cbx_port_sli_mode_t", __cint_enum_map__cbx_port_sli_mode_t },
    { "cbx_cosq_threshold_t", __cint_enum_map__cbx_cosq_threshold_t },
    { "cbx_port_isolation_group_t",
	   __cint_enum_map__cbx_port_isolation_group_t },
    { "cbx_port_attr_t", __cint_enum_map__cbx_port_attr_t },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_fsal_port_typedefs[] =
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
        "cbx_portid_t",
        0,
        0
    },
    {
        "uint32",
        "cbx_port_t",
        0,
        0
    },
    {
        "unsigned short",
        "uint16_t",
        0,
        0
    },
    {
        "uint16_t",
        "cbx_vlan_t",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_constants_t __cint_fsal_port_constants[] =
{
   { "CBX_PORT_INVALID", CBX_PORT_INVALID },
   { "CBX_PORT_VIRTUAL_PORTS_ENABLE", CBX_PORT_VIRTUAL_PORTS_ENABLE },
   { "CBX_PORT_DROP_UNTAGGED", CBX_PORT_DROP_UNTAGGED },
   { "CBX_PORT_DROP_TAGGED", CBX_PORT_DROP_TAGGED },
   { "CBX_PORT_DROP_ULF", CBX_PORT_DROP_ULF },
   { "CBX_PORT_DROP_MLF", CBX_PORT_DROP_MLF },
   { "CBX_PORT_DROP_SLF", CBX_PORT_DROP_SLF },
   { "CBX_PORT_DROP_BCAST", CBX_PORT_DROP_BCAST },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_port_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_port_functions[] =
    {


        CINT_FWRAPPER_ENTRY(cbx_port_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_port_create),
        CINT_FWRAPPER_ENTRY(cbx_port_destroy),
        CINT_FWRAPPER_ENTRY(cbx_port_info_get),
        CINT_FWRAPPER_ENTRY(cbx_port_info_set),
        CINT_FWRAPPER_ENTRY(cbx_port_lookup),
        CINT_FWRAPPER_ENTRY(cbx_port_isolation_get),
        CINT_FWRAPPER_ENTRY(cbx_port_isolation_set),
        CINT_FWRAPPER_ENTRY(cbx_port_attribute_get),
        CINT_FWRAPPER_ENTRY(cbx_port_attribute_set),
        CINT_FWRAPPER_ENTRY(cbx_port_phy_get),
        CINT_FWRAPPER_ENTRY(cbx_port_phy_set),
        CINT_FWRAPPER_ENTRY(cbxi_port_loopback_set),
        CINT_FWRAPPER_ENTRY(cbxi_port_phy_info),
        CINT_FWRAPPER_ENTRY(cbxi_trap_dest_update),
#ifdef CONFIG_CASCADED_MODE
        CINT_FWRAPPER_ENTRY(cbxi_cascade_port_delete),
#endif
        CINT_FWRAPPER_ENTRY(cbxi_port_display_eye_scan),
        CINT_ENTRY_LAST
    };
cint_data_t fsal_port_cint_data =
    {
        NULL,
        __cint_fsal_port_functions,
        __cint_fsal_port_structures,
        __cint_fsal_port_enums,
        __cint_fsal_port_typedefs,
        __cint_fsal_port_constants,
        __cint_fsal_port_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */
