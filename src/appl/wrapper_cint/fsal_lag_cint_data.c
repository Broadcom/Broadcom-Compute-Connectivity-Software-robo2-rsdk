/*
 *
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * fsal_lag_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_lag_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <lag.h>

CINT_FWRAPPER_CREATE_VP1(cbx_lag_psc_t_init,
                         cbx_lag_psc_t*, cbx_lag_psc_t, lag_psc, 1, 0);
CINT_FWRAPPER_CREATE_VP1(cbx_lag_params_t_init,
                         cbx_lag_params_t*, cbx_lag_params_t, lag_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_lag_create,
                         cbx_lag_params_t*, cbx_lag_params_t, lag_params, 1, 0,
                         cbx_portid_t*, cbx_portid_t, lagid, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_lag_destroy,
                         cbx_portid_t, cbx_portid_t, lagid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_lag_member_add,
                         cbx_portid_t, cbx_portid_t, lagid, 0, 0,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_lag_member_remove,
                         cbx_portid_t, cbx_portid_t, lagid, 0, 0,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_lag_member_get,
                         cbx_portid_t, cbx_portid_t, lagid, 0, 0,
                         cbx_lag_member_info_t*, cbx_lag_member_info_t,
						 lag_info, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_lag_psc_get,
                         cbx_portid_t, cbx_portid_t, lagid, 0, 0,
                         cbx_lag_psc_t*, cbx_lag_psc_t, psc, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_lag_psc_set,
                         cbx_portid_t, cbx_portid_t, lagid, 0, 0,
                         cbx_lag_psc_t*, cbx_lag_psc_t, psc, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_lag_psc_reseed,
                         cbx_portid_t, cbx_portid_t, lagid, 0, 0,
                         cbx_lag_reseed_mode_t, cbx_lag_reseed_mode_t,
						 mode, 0, 0);


static void*
__cint_maddr__cbx_lag_psc_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_lag_psc_t* s = (cbx_lag_psc_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->pkt_type); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_lag_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_lag_params_t* s = (cbx_lag_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->lag_mode); break;
        case 1: rv = &(s->port_group); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_lag_member_info_t(void* p, int mnum,
                                     cint_struct_type_t* parent)
{
    void* rv;
    cbx_lag_member_info_t* s = (cbx_lag_member_info_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->lag_params); break;
        case 1: rv = &(s->max_portid); break;
        case 2: rv = &(s->portid_array); break;
        case 3: rv = &(s->valid_portid); break;
        default: rv = NULL; break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__cbx_lag_psc_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "cbx_packet_type_t",
        "pkt_type",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_lag_params_t[] =
{
    {
        "cbx_lag_mode_t",
        "lag_mode",
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
static cint_parameter_desc_t __cint_struct_members__cbx_lag_member_info_t[] =
{
    {
        "cbx_lag_params_t",
        "lag_params",
        0,
        0
    },
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
static cint_struct_type_t __cint_fsal_lag_structures[] =
{
    {
    "cbx_lag_psc_t",
    sizeof(cbx_lag_psc_t),
    __cint_struct_members__cbx_lag_psc_t,
    __cint_maddr__cbx_lag_psc_t
    },
    {
    "cbx_lag_params_t",
    sizeof(cbx_lag_params_t),
    __cint_struct_members__cbx_lag_params_t,
    __cint_maddr__cbx_lag_params_t
    },
    {
    "cbx_lag_member_info_t",
    sizeof(cbx_lag_member_info_t),
    __cint_struct_members__cbx_lag_member_info_t,
    __cint_maddr__cbx_lag_member_info_t
    },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_lag_mode_t[] =
{
    { "cbxLagModeNormal", cbxLagModeNormal },
    { "cbxLagModeCascade", cbxLagModeCascade },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__cbx_lag_reseed_mode_t[] =
{
    { "cbxLagReseedNormal", cbxLagReseedNormal },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__cbx_packet_type_t[] =
{
    { "cbxPacketIpv4", cbxPacketIpv4 },
    { "cbxPacketIpv6", cbxPacketIpv6 },
    { "cbxPacketOther", cbxPacketOther },
    CINT_ENTRY_LAST
};
static cint_enum_type_t __cint_fsal_lag_enums[] =
{
    { "cbx_lag_mode_t", __cint_enum_map__cbx_lag_mode_t },
    { "cbx_lag_reseed_mode_t", __cint_enum_map__cbx_lag_reseed_mode_t },
    { "cbx_packet_type_t", __cint_enum_map__cbx_packet_type_t },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_fsal_lag_typedefs[] =
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
    CINT_ENTRY_LAST
};


static cint_constants_t __cint_fsal_lag_constants[] =
{
   { "CBX_LAG_ALL", CBX_LAG_ALL },
   { "CBX_LAG_INVALID", CBX_LAG_INVALID },
   { "CBX_LAG_PSC_SRCMAC", CBX_LAG_PSC_SRCMAC },
   { "CBX_LAG_PSC_DSTMAC", CBX_LAG_PSC_DSTMAC },
   { "CBX_LAG_PSC_VLAN", CBX_LAG_PSC_VLAN },
   { "CBX_LAG_PSC_SRCIP", CBX_LAG_PSC_SRCIP },
   { "CBX_LAG_PSC_DSTIP", CBX_LAG_PSC_DSTIP },
   { "CBX_LAG_PSC_SRCPORT", CBX_LAG_PSC_SRCPORT },
   { "CBX_LAG_PSC_DSTPORT", CBX_LAG_PSC_DSTPORT },
   { "CBX_LAG_PSC_SRCIPV6", CBX_LAG_PSC_SRCIPV6 },
   { "CBX_LAG_PSC_DSTIPV6", CBX_LAG_PSC_DSTIPV6 },
   { "CBX_LAG_PSC_SPG", CBX_LAG_PSC_SPG },
   { "CBX_LAG_PSC_L3PROTO", CBX_LAG_PSC_L3PROTO },
   { "CBX_LAG_PSC_L4BYTES", CBX_LAG_PSC_L4BYTES },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_lag_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_lag_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_lag_psc_t_init),
        CINT_FWRAPPER_ENTRY(cbx_lag_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_lag_create),
        CINT_FWRAPPER_ENTRY(cbx_lag_destroy),
        CINT_FWRAPPER_ENTRY(cbx_lag_member_add),
        CINT_FWRAPPER_ENTRY(cbx_lag_member_remove),
        CINT_FWRAPPER_ENTRY(cbx_lag_member_get),
        CINT_FWRAPPER_ENTRY(cbx_lag_psc_get),
        CINT_FWRAPPER_ENTRY(cbx_lag_psc_set),
        CINT_FWRAPPER_ENTRY(cbx_lag_psc_reseed),
        CINT_ENTRY_LAST
    };

cint_data_t fsal_lag_cint_data =
    {
        NULL,
        __cint_fsal_lag_functions,
        __cint_fsal_lag_structures,
        __cint_fsal_lag_enums,
        __cint_fsal_lag_typedefs,
        __cint_fsal_lag_constants,
        __cint_fsal_lag_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */


