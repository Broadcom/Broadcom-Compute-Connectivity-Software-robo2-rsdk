/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * dot1as_cint_data.c
 *
 * Hand-coded support for 802.1as routines.
 */
int dot1as_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <dot1as.h>

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         dot1asModeSet,
                         DOT1AS_ADMIN_MODE_t, DOT1AS_ADMIN_MODE_t, mode, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         dot1asModeGet,
                         DOT1AS_ADMIN_MODE_t*, DOT1AS_ADMIN_MODE_t, mode, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         dot1asIntfModeSet,
                         uint32_t, uint32_t, intf, 0, 0,
                         DOT1AS_ADMIN_MODE_t, DOT1AS_ADMIN_MODE_t, mode, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         dot1asIntfModeGet,
                         uint32_t, uint32_t, intf, 0, 0,
                         DOT1AS_ADMIN_MODE_t*, DOT1AS_ADMIN_MODE_t, mode, 1, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         dot1asBridgePrioritySet,
                         DOT1AS_CLOCK_PRIORITY_t, DOT1AS_CLOCK_PRIORITY_t, type, 0, 0,
                         uint32_t, uint32_t, priority, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         dot1asIntfIntervalSet,
                         DOT1AS_INTF_INTERVAL_t, DOT1AS_INTF_INTERVAL_t, type, 0, 0,
                         uint32_t, uint32_t, intf, 0, 0,
                         int, int, val, 0, 0);
CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         dot1asIntfAllowedLostRespSet,
                         uint32_t, uint32_t, intf, 0, 0,
                         uint32_t, uint32_t, number, 0, 0);
CINT_FWRAPPER_CREATE_VP0(dot1asDebugBmcaShow);
CINT_FWRAPPER_CREATE_VP2(dot1asDebugPdelayShow,
                         int, int, intf, 0, 0,
                         uint8_t, uint8_t, showlog, 0, 0);
CINT_FWRAPPER_CREATE_VP1(dot1asDebugIntfStatsShow,
                         uint32_t, uint32_t, intf, 0, 0);
CINT_FWRAPPER_CREATE_VP2(dot1asDebugTimesyncShow,
                         uint8_t, uint8_t, verbose, 0, 0,
                         uint32_t, uint32_t, num, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         dot1asIntfStatsReset,
                         uint32_t, uint32_t, intf, 0, 0);
CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                         dot1asDebugIntlShow);
CINT_FWRAPPER_CREATE_VP1(dot1asDebugIntfStatusShow,
                         uint32_t, uint32_t, intf, 0, 0);
CINT_FWRAPPER_CREATE_VP1(dot1asDebugFlagsSet,
                         uint32_t, uint32_t, flags, 0, 0);

static cint_struct_type_t __cint_dot1as_structures[] =
{
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__DOT1AS_ADMIN_MODE_t[] =
{
    { "DOT1AS_DISABLE", DOT1AS_DISABLE },
    { "DOT1AS_ENABLE", DOT1AS_ENABLE },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__DOT1AS_CLOCK_PRIORITY_t[] =
{
    { "DOT1AS_CLOCK_PRIORITY1", DOT1AS_CLOCK_PRIORITY1 },
    { "DOT1AS_CLOCK_PRIORITY2", DOT1AS_CLOCK_PRIORITY2 },
    CINT_ENTRY_LAST
};
static cint_enum_map_t __cint_enum_map__DOT1AS_INTF_INTERVAL_t[] =
{
    { "DOT1AS_INITIAL_SYNC_TX_INTERVAL", DOT1AS_INITIAL_SYNC_TX_INTERVAL },
    { "DOT1AS_CURRENT_SYNC_TX_INTERVAL", DOT1AS_CURRENT_SYNC_TX_INTERVAL },
    { "DOT1AS_INITIAL_ANNOUNCE_TX_INTERVAL", DOT1AS_INITIAL_ANNOUNCE_TX_INTERVAL },
    { "DOT1AS_CURRENT_ANNOUNCE_TX_INTERVAL", DOT1AS_CURRENT_ANNOUNCE_TX_INTERVAL },
    { "DOT1AS_INITIAL_PDELAY_TX_INTERVAL", DOT1AS_INITIAL_PDELAY_TX_INTERVAL },
    { "DOT1AS_CURRENT_PDELAY_TX_INTERVAL", DOT1AS_CURRENT_PDELAY_TX_INTERVAL },
    { "DOT1AS_SYNC_RX_TIMEOUT", DOT1AS_SYNC_RX_TIMEOUT },
    { "DOT1AS_ANNOUNCE_RX_TIMEOUT", DOT1AS_ANNOUNCE_RX_TIMEOUT },
    { "DOT1AS_PDELAY_RX_TIMEOUT", DOT1AS_PDELAY_RX_TIMEOUT },
    { "DOT1AS_SYNC_RX_TIMEOUT_INTERVAL", DOT1AS_SYNC_RX_TIMEOUT_INTERVAL },
    CINT_ENTRY_LAST
};
static cint_enum_type_t __cint_dot1as_enums[] =
{
    { "DOT1AS_ADMIN_MODE_t", __cint_enum_map__DOT1AS_ADMIN_MODE_t},
    { "DOT1AS_CLOCK_PRIORITY_t", __cint_enum_map__DOT1AS_CLOCK_PRIORITY_t},
    { "DOT1AS_INTF_INTERVAL_t", __cint_enum_map__DOT1AS_INTF_INTERVAL_t},
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_dot1as_typedefs[] =
{
    CINT_ENTRY_LAST
};

static cint_constants_t __cint_dot1as_constants[] =
{
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_dot1as_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_dot1as_functions[] =
    {
        CINT_FWRAPPER_ENTRY(dot1asModeSet),
        CINT_FWRAPPER_ENTRY(dot1asModeGet),
        CINT_FWRAPPER_ENTRY(dot1asIntfModeSet),
        CINT_FWRAPPER_ENTRY(dot1asIntfModeGet),
        CINT_FWRAPPER_ENTRY(dot1asBridgePrioritySet),
        CINT_FWRAPPER_ENTRY(dot1asIntfAllowedLostRespSet),
        CINT_FWRAPPER_ENTRY(dot1asIntfIntervalSet),
        CINT_FWRAPPER_ENTRY(dot1asDebugBmcaShow),
        CINT_FWRAPPER_ENTRY(dot1asDebugPdelayShow),
        CINT_FWRAPPER_ENTRY(dot1asDebugIntfStatsShow),
        CINT_FWRAPPER_ENTRY(dot1asDebugTimesyncShow),
        CINT_FWRAPPER_ENTRY(dot1asIntfStatsReset),
        CINT_FWRAPPER_ENTRY(dot1asDebugIntlShow),
        CINT_FWRAPPER_ENTRY(dot1asDebugIntfStatusShow),
        CINT_FWRAPPER_ENTRY(dot1asDebugFlagsSet),
        CINT_ENTRY_LAST
    };
cint_data_t dot1as_cint_data =
    {
        NULL,
        __cint_dot1as_functions,
        __cint_dot1as_structures,
        __cint_dot1as_enums,
        __cint_dot1as_typedefs,
        __cint_dot1as_constants,
        __cint_dot1as_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */
