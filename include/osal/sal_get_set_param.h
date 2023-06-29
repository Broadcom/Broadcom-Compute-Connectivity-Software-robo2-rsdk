/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File: sal_get_set_param.h
 * Purpose: Apis to get and set params to NVM
 */

#ifndef __SAL_GET_SET_PARAM_H__
#define __SAL_GET_SET_PARAM_H__

#include "sal_types.h" 
#include "soc/error.h" 

/* Mac address parameter name - Used to set/get mac address from cli */
#define MAC_ADDRESS_PARAM_NAME  "mac_address"
#define LOGIN_PASSWORD_PARAM_NAME "pwd"

/* Parameter file name - stored in read/write file system */
#if defined (WEB_SERVER_SIM) || defined(CONFIG_EXTERNAL_HOST)
  #define CFG_FILE_DRIVE      "."
#else
  #define CFG_FILE_DRIVE      "cfg:"
#endif
#define PARAMS_FILE          CFG_FILE_DRIVE "/params.txt"

soc_error_t osal_get_param(const char *file, const char *param, char *value, uint32_t value_len);
soc_error_t osal_set_param(const char *file, const char *param, const char *value);

#endif /* !__SAL_GET_SET_PARAM_H__ */
