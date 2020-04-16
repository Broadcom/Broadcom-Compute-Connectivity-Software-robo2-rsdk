/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * error.h
 */

#ifndef _SOC_ERROR_H
#define _SOC_ERROR_H

#include <shared/error.h>

/*
 * Defines:	SOC_E_XXX
 * Purpose:	SOC API error codes
 * Notes:
 *      An error code may be converted to a string by passing
 *      the code to soc_errmsg().
 */

typedef enum {
    SOC_E_NONE                  = _SHR_E_NONE,
    SOC_E_INTERNAL              = _SHR_E_INTERNAL,
    SOC_E_MEMORY                = _SHR_E_MEMORY,
    SOC_E_UNIT                  = _SHR_E_UNIT,
    SOC_E_PARAM                 = _SHR_E_PARAM,
    SOC_E_EMPTY                 = _SHR_E_EMPTY,
    SOC_E_FULL                  = _SHR_E_FULL,
    SOC_E_NOT_FOUND             = _SHR_E_NOT_FOUND,
    SOC_E_EXISTS                = _SHR_E_EXISTS,
    SOC_E_TIMEOUT               = _SHR_E_TIMEOUT,
    SOC_E_BUSY                  = _SHR_E_BUSY,
    SOC_E_FAIL                  = _SHR_E_FAIL,
    SOC_E_DISABLED              = _SHR_E_DISABLED,
    SOC_E_BADID                 = _SHR_E_BADID,
    SOC_E_RESOURCE              = _SHR_E_RESOURCE,
    SOC_E_CONFIG                = _SHR_E_CONFIG,
    SOC_E_UNAVAIL               = _SHR_E_UNAVAIL,
    SOC_E_INIT                  = _SHR_E_INIT,
    SOC_E_PORT                  = _SHR_E_PORT,
    SOC_E_LIMIT                 = _SHR_E_LIMIT
} soc_error_t;

#define SOC_SUCCESS(rv)         _SHR_E_SUCCESS(rv)
#define SOC_FAILURE(rv)         _SHR_E_FAILURE(rv)
#define SOC_IF_ERROR_RETURN(op) _SHR_E_IF_ERROR_RETURN(op)
#define soc_errmsg(rv)          _SHR_ERRMSG(rv)
#define SOC_IF_ERROR_CLEAN_RETURN(op,exop) _SHR_E_IF_ERROR_CLEAN_RETURN(op,exop)


#endif  /* !_SOC_ERROR_H */
