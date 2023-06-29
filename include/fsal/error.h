/*
 * $Copyright: (c) 2017 Broadcom Limited.
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 * Error code definitions for FSAL layer
 *
 */

#ifndef __CBX_ERROR_H__
#define __CBX_ERROR_H__

#include <shared/error.h>

/**
 * CBX API error codes.
 *
 * @note An error code may be converted to a string
 *  by passing the code to cbx_errmsg().
 */
typedef enum cbx_error_e {
    CBX_E_NONE         = _SHR_E_NONE,
    CBX_E_INTERNAL     = _SHR_E_INTERNAL,
    CBX_E_MEMORY       = _SHR_E_MEMORY,
    CBX_E_UNIT         = _SHR_E_UNIT,
    CBX_E_PARAM        = _SHR_E_PARAM,
    CBX_E_EMPTY        = _SHR_E_EMPTY,
    CBX_E_FULL         = _SHR_E_FULL,
    CBX_E_NOT_FOUND    = _SHR_E_NOT_FOUND,
    CBX_E_EXISTS       = _SHR_E_EXISTS,
    CBX_E_TIMEOUT      = _SHR_E_TIMEOUT,
    CBX_E_BUSY         = _SHR_E_BUSY,
    CBX_E_FAIL         = _SHR_E_FAIL,
    CBX_E_DISABLED     = _SHR_E_DISABLED,
    CBX_E_BADID        = _SHR_E_BADID,
    CBX_E_RESOURCE     = _SHR_E_RESOURCE,
    CBX_E_CONFIG       = _SHR_E_CONFIG,
    CBX_E_UNAVAIL      = _SHR_E_UNAVAIL,
    CBX_E_INIT         = _SHR_E_INIT,
    CBX_E_PORT         = _SHR_E_PORT
} cbx_error_t;

/**
 * Switch event types
 */
typedef enum cbx_switch_event_e {
    CBX_SWITCH_EVENT_IO_ERROR              =   _SHR_SWITCH_EVENT_IO_ERROR,
    CBX_SWITCH_EVENT_PARITY_ERROR          =   _SHR_SWITCH_EVENT_PARITY_ERROR,
    CBX_SWITCH_EVENT_THREAD_ERROR          =   _SHR_SWITCH_EVENT_THREAD_ERROR,
    CBX_SWITCH_EVENT_ACCESS_ERROR          =   _SHR_SWITCH_EVENT_ACCESS_ERROR,
    CBX_SWITCH_EVENT_ASSERT_ERROR          =   _SHR_SWITCH_EVENT_ASSERT_ERROR,
    CBX_SWITCH_EVENT_MODID_CHANGE          =   _SHR_SWITCH_EVENT_MODID_CHANGE,
    CBX_SWITCH_EVENT_DOS_ATTACK            =   _SHR_SWITCH_EVENT_DOS_ATTACK,
    CBX_SWITCH_EVENT_STABLE_FULL           =   _SHR_SWITCH_EVENT_STABLE_FULL,
    CBX_SWITCH_EVENT_STABLE_ERROR          =   _SHR_SWITCH_EVENT_STABLE_ERROR,
    CBX_SWITCH_EVENT_UNCONTROLLED_SHUTDOWN =
        _SHR_SWITCH_EVENT_UNCONTROLLED_SHUTDOWN,
    CBX_SWITCH_EVENT_WARM_BOOT_DOWNGRADE   =
        _SHR_SWITCH_EVENT_WARM_BOOT_DOWNGRADE,
    CBX_SWITCH_EVENT_TUNE_ERROR            =   _SHR_SWITCH_EVENT_TUNE_ERROR,
    CBX_SWITCH_EVENT_DEVICE_INTERRUPT      =
        _SHR_SWITCH_EVENT_DEVICE_INTERRUPT,
    CBX_SWITCH_EVENT_ALARM                 =   _SHR_SWITCH_EVENT_ALARM,
    CBX_SWITCH_EVENT_MMU_BST_TRIGGER       =
        _SHR_SWITCH_EVENT_MMU_BST_TRIGGER,
    CBX_SWITCH_EVENT_EPON_ALARM            =   _SHR_SWITCH_EVENT_EPON_ALARM,
    CBX_SWITCH_EVENT_RUNT_DETECT           =   _SHR_SWITCH_EVENT_RUNT_DETECT,
    CBX_SWITCH_EVENT_COUNT                 =   _SHR_SWITCH_EVENT_COUNT
} cbx_switch_event_t;

/** @{
 * CBX Return Code handling
 */
#define CBX_SUCCESS(rv)         \
    _SHR_E_SUCCESS(rv)

#define CBX_FAILURE(rv)         \
    _SHR_E_FAILURE(rv)

#define CBX_IF_ERROR_RETURN(op)  \
    _SHR_E_IF_ERROR_RETURN(op)

#define CBX_IF_ERROR_NOT_UNAVAIL_RETURN(op)  \
    _SHR_E_IF_ERROR_NOT_UNAVAIL_RETURN(op)

#define cbx_errmsg(rv)          \
    _SHR_ERRMSG(rv)
/** @} */

#endif /* __CBX_ERROR_H__ */
