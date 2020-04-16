/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * fsal_cable_diag_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_cable_diag_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <cable_diag.h>
#include <fsal_int/cable_diag.h>

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_port_cable_diag,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_port_cable_diag_t *, cbx_port_cable_diag_t,
						 port_result, 1, 0);
CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                         cbxi_cdiag_run_all);


static void*
__cint_maddr__cbx_port_cable_diag_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_port_cable_diag_t* s = (cbx_port_cable_diag_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->port_cdiag_result); break;
        case 1: rv = &(s->pair_state); break;
        case 2: rv = &(s->pair_len); break;
        default: rv = NULL; break;
    }
    return rv;
}
static cint_parameter_desc_t __cint_struct_members__cbx_port_cable_diag_t[] =
{
    {
        "cbx_cdiag_result_t",
        "port_cdiag_result",
        0,
        0
    },
    {
        "cbx_cdiag_state_t",
        "pair_state",
        0,
        4
    },
    {
        "uint32_t",
        "port_len",
        0,
        4
    },
    CINT_ENTRY_LAST
};

static cint_struct_type_t __cint_fsal_cable_diag_structures[] =
{
    {
    "cbx_port_cable_diag_t",
    sizeof(cbx_port_cable_diag_t),
    __cint_struct_members__cbx_port_cable_diag_t,
    __cint_maddr__cbx_port_cable_diag_t
    },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_cdiag_result_t[] =
{
    { "cbxCdiagResultNill", cbxCdiagResultNill },
    { "cbxCdiagResultBusy", cbxCdiagResultBusy },
    { "cbxCdiagResultInvalid", cbxCdiagResultInvalid },
    { "cbxCdiagResultPass", cbxCdiagResultPass },
    { "cbxCdiagResultFail", cbxCdiagResultFail},
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_cdiag_state_t[] =
{
    { "cbxCableStateInvalid", cbxCableStateInvalid },
    { "cbxCableStateOK", cbxCableStateOK },
    { "cbxCableStateOpen", cbxCableStateOpen },
    { "cbxCableStateIntraShort", cbxCableStateIntraShort },
    { "cbxCableStateInterShort", cbxCableStateInterShort },
    { "cbxCableStateBusy", cbxCableStateBusy },
    CINT_ENTRY_LAST
};


static cint_enum_type_t __cint_fsal_cable_diag_enums[] =
{
    { "cbx_cdiag_result_t", __cint_enum_map__cbx_cdiag_result_t },
    { "cbx_cdiag_state_t", __cint_enum_map__cbx_cdiag_state_t },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_fsal_cable_diag_typedefs[] =
{
    CINT_ENTRY_LAST
};


static cint_constants_t __cint_fsal_cable_diag_constants[] =
{
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_cable_diag_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_cable_diag_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_port_cable_diag),
        CINT_FWRAPPER_ENTRY(cbxi_cdiag_run_all),
        CINT_ENTRY_LAST
    };

cint_data_t fsal_cable_diag_cint_data =
    {
        NULL,
        __cint_fsal_cable_diag_functions,
        __cint_fsal_cable_diag_structures,
        __cint_fsal_cable_diag_enums,
        __cint_fsal_cable_diag_typedefs,
        __cint_fsal_cable_diag_constants,
        __cint_fsal_cable_diag_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */


