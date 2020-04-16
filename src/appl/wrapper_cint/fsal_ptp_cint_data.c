/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * fsal_ptp_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_ptp_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <ptp.h>

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_ptp_clock_mode_set,
                         cbx_ptp_clk_mode_t, cbx_ptp_clk_mode_t, mode, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_ptp_clock_mode_get,
                         cbx_ptp_clk_mode_t *, cbx_ptp_clk_mode_t, mode, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_ptp_port_admin_set,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         uint8_t, uint8_t, admin, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_ptp_port_admin_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         uint8_t *, uint8_t, admin, 1, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_ptp_timestamp_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         uint32_t *, uint32_t, time_stamp, 1, 0,
                         uint32_t *, uint32_t, seq_id, 1, 0);

static cint_struct_type_t __cint_fsal_ptp_structures[] =
{
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_ptp_clock_mode_t[] =
{
    { "cbxPtpClkOneStep", cbxPtpClkOneStep },
    { "cbxPtpClkTwoStep", cbxPtpClkTwoStep },
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_fsal_ptp_enums[] =
{
    { "cbx_ptp_clk_mode_t", __cint_enum_map__cbx_ptp_clock_mode_t },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_fsal_ptp_typedefs[] =
{
    CINT_ENTRY_LAST
};

static cint_constants_t __cint_fsal_ptp_constants[] =
{
    { "CBX_PTP_PORT_ENABLE", CBX_PTP_PORT_ENABLE },
    { "CBX_PTP_PORT_DISABLE", CBX_PTP_PORT_DISABLE },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_ptp_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_ptp_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_ptp_clock_mode_set),
        CINT_FWRAPPER_ENTRY(cbx_ptp_clock_mode_get),
        CINT_FWRAPPER_ENTRY(cbx_ptp_port_admin_set),
        CINT_FWRAPPER_ENTRY(cbx_ptp_port_admin_get),
        CINT_FWRAPPER_ENTRY(cbx_ptp_timestamp_get),
        CINT_ENTRY_LAST
    };
cint_data_t fsal_ptp_cint_data =
    {
        NULL,
        __cint_fsal_ptp_functions,
        __cint_fsal_ptp_structures,
        __cint_fsal_ptp_enums,
        __cint_fsal_ptp_typedefs,
        __cint_fsal_ptp_constants,
        __cint_fsal_ptp_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */
