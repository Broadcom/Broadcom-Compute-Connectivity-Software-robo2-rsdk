/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * fsal_link_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_link_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <link.h>

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_link_scan_init,
                         cbx_link_scan_params_t *, cbx_link_scan_params_t,
                         link_scan_params, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_link_scan_detach,
                         cbx_link_scan_params_t *, cbx_link_scan_params_t,
                         link_scan_params, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_link_scan_register,
                         cbx_link_scan_info_t, cbx_link_scan_info_t,
                         link_scan_info, 0, 0,
                         cbx_link_scan_callback_t, cbx_link_scan_callback_t,
                         cb, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_link_scan_unregister,
                         cbx_link_scan_callback_t, cbx_link_scan_callback_t,
                         cb, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_link_scan_enable_get,
                         cbx_link_scan_params_t *, cbx_link_scan_params_t,
                         link_scan_params, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_link_scan_enable_set,
                         cbx_link_scan_params_t *, cbx_link_scan_params_t,
                         link_scan_params, 1, 0);
CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                         cbx_link_scan_dump);


static void*
__cint_maddr__cbx_link_scan_params_t(void* p, int mnum, cint_struct_type_t* parent
)
{
    void* rv;
    cbx_link_scan_params_t* s = (cbx_link_scan_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->usec); break;
        default: rv = NULL; break;
    }
    return rv;
}


static void*
__cint_maddr__cbx_link_scan_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_link_scan_info_t* s = (cbx_link_scan_info_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->name); break;
        default: rv = NULL; break;
    }
    return rv;
}
static cint_parameter_desc_t __cint_struct_members__cbx_link_scan_params_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "int",
        "usec",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_link_scan_info_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "char",
        "name",
        1,
        0
    },
    CINT_ENTRY_LAST
};

static cint_struct_type_t __cint_fsal_link_structures[] =
{
    {
    "cbx_link_scan_params_t",
    sizeof(cbx_link_scan_params_t),
    __cint_struct_members__cbx_link_scan_params_t,
    __cint_maddr__cbx_link_scan_params_t
    },
    {
    "cbx_link_scan_info_t",
    sizeof(cbx_link_scan_info_t),
    __cint_struct_members__cbx_link_scan_info_t,
    __cint_maddr__cbx_link_scan_info_t
    },
    CINT_ENTRY_LAST
};


static cint_enum_type_t __cint_fsal_link_enums[] =
{
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_fsal_link_typedefs[] =
{
    CINT_ENTRY_LAST
};


static cint_constants_t __cint_fsal_link_constants[] =
{
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_function_pointers[2];
static int
__cint_fpointer__link_cb(cbx_portid_t  portid, uint32_t value)
{
  int returnval;

  cint_interpreter_callback(__cint_function_pointers+0, 2, 1, &portid, &value, &returnval);

  return returnval;
}
static cint_parameter_desc_t __cint_parameters_link_cb[] =
{
        {
            "int",
            "r",
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
        "uint32_t",
        "value",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_link_function_pointers[] =
{
    {
        "cbx_link_scan_callback_t",
        (cint_fpointer_t) __cint_fpointer__link_cb,
        __cint_parameters_link_cb,
    },
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_link_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_link_scan_init),
        CINT_FWRAPPER_ENTRY(cbx_link_scan_detach),
        CINT_FWRAPPER_ENTRY(cbx_link_scan_register),
        CINT_FWRAPPER_ENTRY(cbx_link_scan_unregister),
        CINT_FWRAPPER_ENTRY(cbx_link_scan_enable_get),
        CINT_FWRAPPER_ENTRY(cbx_link_scan_enable_set),
        CINT_FWRAPPER_ENTRY(cbx_link_scan_dump),
        CINT_ENTRY_LAST
    };

cint_data_t fsal_link_cint_data =
    {
        NULL,
        __cint_fsal_link_functions,
        __cint_fsal_link_structures,
        __cint_fsal_link_enums,
        __cint_fsal_link_typedefs,
        __cint_fsal_link_constants,
        __cint_fsal_link_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */


