/*
 *
 * $Copyright: (c) 2016 Broadcom Ltd
 * All Rights Reserved.$
 *
 * osal_cint_data.c
 *
 * Hand-coded support for OSAL routines.
 */
int osal_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_time.h>
#include <sal_get_set_param.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <cint_config.h>

CINT_FWRAPPER_CREATE_VP1(sal_sleep,
                         int, int, sec, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         osal_set_param,
                         char*, char, file, 1, 0,
                         char*, char, param, 1, 0,
                         char*, char, value, 1, 0);
CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         osal_get_param,
                         char*, char, file, 1, 0,
                         char*, char, param, 1, 0,
                         char*, char, value, 1, 0,
                         uint32_t, uint32_t, value_len, 0, 0);

static cint_struct_type_t __cint_osal_structures[] =
{
    CINT_ENTRY_LAST
};


static cint_enum_type_t __cint_osal_enums[] =
{
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_osal_typedefs[] =
{
    CINT_ENTRY_LAST
};


static cint_constants_t __cint_osal_constants[] =
{
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_osal_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_osal_functions[] =
    {
        CINT_FWRAPPER_ENTRY(sal_sleep),
        CINT_FWRAPPER_ENTRY(osal_set_param),
        CINT_FWRAPPER_ENTRY(osal_get_param),
        CINT_ENTRY_LAST
    };

cint_data_t osal_cint_data =
    {
        NULL,
        __cint_osal_functions,
        __cint_osal_structures,
        __cint_osal_enums,
        __cint_osal_typedefs,
        __cint_osal_constants,
        __cint_osal_function_pointers
    };
#endif /* INCLUDE_LIB_CINT */
