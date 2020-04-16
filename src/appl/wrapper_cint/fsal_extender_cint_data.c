/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * fsal_extender_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_extender_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <extender.h>

CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                         cbx_extender_init);
CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                         cbx_extender_detach);

static cint_struct_type_t __cint_fsal_extender_structures[] =
{
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_fsal_extender_enums[] =
{
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_fsal_extender_typedefs[] =
{
    CINT_ENTRY_LAST
};

static cint_constants_t __cint_fsal_extender_constants[] =
{
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_extender_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_extender_functions[] =
    {

        CINT_FWRAPPER_ENTRY(cbx_extender_init),
        CINT_FWRAPPER_ENTRY(cbx_extender_detach),

        CINT_ENTRY_LAST
    };
cint_data_t fsal_extender_cint_data =
    {
        NULL,
        __cint_fsal_extender_functions,
        __cint_fsal_extender_structures,
        __cint_fsal_extender_enums,
        __cint_fsal_extender_typedefs,
        __cint_fsal_extender_constants,
        __cint_fsal_extender_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */
