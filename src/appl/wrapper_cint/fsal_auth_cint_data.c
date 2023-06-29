/*
 *
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * fsal_auth_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_auth_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <auth.h>


CINT_FWRAPPER_CREATE_VP1(cbx_auth_params_t_init,
                         cbx_auth_params_t*, cbx_auth_params_t,
                         auth_params, 1, 0);
CINT_FWRAPPER_CREATE_VP1(cbx_auth_port_mode_t_init,
                         cbx_auth_port_mode_t*, cbx_auth_port_mode_t,
                         auth_port_mode, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_auth_init,
                         cbx_auth_params_t*, cbx_auth_params_t,
                         auth_params, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_auth_detach,
                         cbx_auth_params_t*, cbx_auth_params_t,
                         auth_params, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_auth_mode_get,
                         cbx_auth_params_t*, cbx_auth_params_t,
                         auth_params, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_auth_mode_set,
                         cbx_auth_params_t*, cbx_auth_params_t,
                         auth_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_auth_port_get,
                         cbx_portid_t, cbx_portid_t,
                         portid, 0, 0,
                         cbx_auth_port_mode_t*, cbx_auth_port_mode_t,
                         port_mode, 1, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_auth_port_set,
                         cbx_portid_t, cbx_portid_t,
                         portid, 0, 0,
                         cbx_auth_port_mode_t*, cbx_auth_port_mode_t,
                         port_mode, 1, 0);

static void*
__cint_maddr__cbx_auth_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_auth_params_t* s = (cbx_auth_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->auth_mode); break;
        case 2: rv = &(s->uc_mac); break;
        case 3: rv = &(s->mc_mac); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_auth_port_mode_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_auth_port_mode_t* s = (cbx_auth_port_mode_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        default: rv = NULL; break;
    }
    return rv;
}
static cint_parameter_desc_t __cint_struct_members__cbx_auth_params_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "cbx_auth_mode_t",
        "auth_mode",
        0,
        0
    },
    {
        "eth_mac_t",
        "uc_mac",
        0,
        0
    },
    {
        "eth_mac_t",
        "mc_mac",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_auth_port_mode_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_struct_type_t __cint_fsal_auth_structures[] =
{
    {
    "cbx_auth_params_t",
    sizeof(cbx_auth_params_t),
    __cint_struct_members__cbx_auth_params_t,
    __cint_maddr__cbx_auth_params_t
    },
    {
    "cbx_auth_port_mode_t",
    sizeof(cbx_auth_port_mode_t),
    __cint_struct_members__cbx_auth_port_mode_t,
    __cint_maddr__cbx_auth_port_mode_t
    },
    CINT_ENTRY_LAST
};


static cint_enum_map_t __cint_enum_map__cbx_auth_mode_t[] =
{
    { "cbxAuthModeUncontrolled", cbxAuthModeUncontrolled },
    { "cbxAuthModeUnauth", cbxAuthModeUnauth },
    { "cbxAuthModeAuth", cbxAuthModeAuth },
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_fsal_auth_enums[] =
{
    { "cbx_auth_mode_t", __cint_enum_map__cbx_auth_mode_t },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_fsal_auth_typedefs[] =
{
    CINT_ENTRY_LAST
};

static cint_constants_t __cint_fsal_auth_constants[] =
{
   { "CBX_AUTH_PORT_UNAUTH", CBX_AUTH_PORT_UNAUTH },
   { "CBX_AUTH_PORT_BLOCK_IN", CBX_AUTH_PORT_BLOCK_IN },
   { "CBX_AUTH_PORT_BLOCK_INOUT", CBX_AUTH_PORT_BLOCK_INOUT },
   { "CBX_AUTH_PORT_AUTH", CBX_AUTH_PORT_AUTH },
   { "CBX_AUTH_PORT_UNKNOWN_DROP", CBX_AUTH_PORT_UNKNOWN_DROP },
   { "CBX_AUTH_PORT_UNKNOWN_TRAP", CBX_AUTH_PORT_UNKNOWN_TRAP },
   { "CBX_AUTH_PORT_LINK", CBX_AUTH_PORT_LINK },
    CINT_ENTRY_LAST
};
static cint_function_pointer_t __cint_fsal_auth_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_auth_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_auth_params_t_init),
        CINT_FWRAPPER_ENTRY(cbx_auth_port_mode_t_init),
        CINT_FWRAPPER_ENTRY(cbx_auth_init),
        CINT_FWRAPPER_ENTRY(cbx_auth_detach),
        CINT_FWRAPPER_ENTRY(cbx_auth_mode_get),
        CINT_FWRAPPER_ENTRY(cbx_auth_mode_set),
        CINT_FWRAPPER_ENTRY(cbx_auth_port_get),
        CINT_FWRAPPER_ENTRY(cbx_auth_port_set),
        CINT_ENTRY_LAST
    };

cint_data_t fsal_auth_cint_data =
    {
        NULL,
        __cint_fsal_auth_functions,
        __cint_fsal_auth_structures,
        __cint_fsal_auth_enums,
        __cint_fsal_auth_typedefs,
        __cint_fsal_auth_constants,
        __cint_fsal_auth_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */


