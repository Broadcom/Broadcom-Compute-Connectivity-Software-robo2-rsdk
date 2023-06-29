/*
 * $Id$
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:    avb_cfgx.c
 */

#include "osal/sal_types.h"
#include "osal/sal_alloc.h"
#include "osal/sal_console.h"
#include "shared/types.h"
#include "avb_cfgx.h"
#include "dot1as_cfgx.h"

/* Debug flag and macro */
uint32_t cfgxTraceMask = (CFGX_TRACE_ERR_FLAG | CFGX_TRACE_WARN_FLAG);

uint8_t *avbCfgxIntfNum = NULL;

AVB_RC_t cfgxIntfIndexGet(uint16_t intfNum, uint8_t *intfIndex)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  i;

  for (i=0; i < avbMaxIntfaces_g; i++)
  {
    if (intfNum == avbCfgxIntfNum[i])
    {
      *intfIndex = i;
      rc = AVB_SUCCESS;
      break;
    }
  }
  if (rc != AVB_SUCCESS)
  {
    CFGX_TRACE_WARN("Warning: (%s) Invalid interface %u", __FUNCTION__, intfNum);
  }
  return rc;
}

void cfgxIntfInit(intf_cfgx_t *cfgx, uint16_t key, uint8_t type, uint16_t val, uint16_t val_default, uint8_t flags)
{
  cfgx->key = key;
  cfgx->type = type;  /* TRGTRG TODO */
  cfgx->val = val;
  cfgx->val_boot = val;
  cfgx->val_default = val_default;
  cfgx->flags = flags;
  cfgx->valid = AVB_TRUE;
}

AVB_RC_t cfgxIntfSet(intf_cfgx_t *cfgx, uint16_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  i;

  if (cfgx->valid != AVB_TRUE)
  {
    CFGX_TRACE_ERR(CFGX_TRACE_ERR_FMT_UNINITIALIZED_ELEMENT, __FUNCTION__, cfgx->key);
    rc = AVB_FAILURE;
  }
  for (i=0; i < avbMaxIntfaces_g; i++)
  {
    if (val == avbCfgxIntfNum[i])
    {
      cfgx->val = val;
      rc = AVB_SUCCESS;
      break;
    }
  }
  return rc;
}


void cfgxBoolInit(bool_cfgx_t *cfgx, uint16_t key, uint8_t val, uint8_t val_default)
{
  if ((val != AVB_FALSE) && (val != AVB_TRUE))
  {
    CFGX_TRACE_WARN("Warning: Invalid boolean config value for key 0x%04x, initializing to default", key);
    val = val_default;
  }
  cfgx->key = key;
  cfgx->val = val;
  cfgx->val_boot = val;
  cfgx->val_default = val_default;
  cfgx->valid = AVB_TRUE;
}

AVB_RC_t cfgxBoolSet(bool_cfgx_t *cfgx, uint8_t val)
{
  AVB_RC_t rc = AVB_SUCCESS;

  if (cfgx->valid != AVB_TRUE)
  {
    CFGX_TRACE_ERR(CFGX_TRACE_ERR_FMT_UNINITIALIZED_ELEMENT, __FUNCTION__, cfgx->key);
    rc = AVB_FAILURE;
  }
  else if ((val != AVB_FALSE) && (val != AVB_TRUE))
  {
    rc = AVB_FAILURE;
  }
  else
  {
    cfgx->val = val;
  }
  return rc;
}


void cfgxUint8Init(uint8_cfgx_t *cfgx, uint16_t key, uint8_t val, uint8_t min, uint8_t max, uint8_t val_default, uint8_t flags)
{
  if ((flags & CFGX_FLAG_INIT_NOCHECK) != CFGX_FLAG_INIT_NOCHECK) /* Give the option to turn off checking for initialization */
  {
    if ((val < min) || (val > max))
    {
      CFGX_TRACE_WARN("Warning: Value out of range for key 0x%04x, initializing to default", key);
      val = val_default;
    }
  }
  cfgx->key = key;
  cfgx->val = val;
  cfgx->val_boot = val;
  cfgx->val_default = val_default;
  cfgx->min = min;
  cfgx->max = max;
  cfgx->flags = flags;
  cfgx->valid = AVB_TRUE;
}

AVB_RC_t cfgxUint8Set(uint8_cfgx_t *cfgx, uint8_t val)
{
  AVB_RC_t rc = AVB_SUCCESS;

  if (cfgx->valid != AVB_TRUE)
  {
    CFGX_TRACE_ERR(CFGX_TRACE_ERR_FMT_UNINITIALIZED_ELEMENT, __FUNCTION__, cfgx->key);
    rc = AVB_FAILURE;
  }
  else if ((val < cfgx->min) || (val > cfgx->max))
  {
    rc = AVB_FAILURE;
  }
  else
  {
    cfgx->val = val;
  }
  return rc;
}

void cfgxInt8Init(int8_cfgx_t *cfgx, uint16_t key, int8_t val, int8_t min, int8_t max, int8_t val_default, uint8_t flags)
{
  if ((flags & CFGX_FLAG_INIT_NOCHECK) != CFGX_FLAG_INIT_NOCHECK) /* Give the option to turn off checking for initialization */
  {
    if ((val < min) || (val > max))
    {
      CFGX_TRACE_WARN("Warning: Value out of range for key 0x%04x, initializing to default", key);
      val = val_default;
    }
  }
  cfgx->key = key;
  cfgx->val = val;
  cfgx->val_boot = val;
  cfgx->val_default = val_default;
  cfgx->min = min;
  cfgx->max = max;
  cfgx->flags = flags;
  cfgx->valid = AVB_TRUE;
}

AVB_RC_t cfgxInt8Set(int8_cfgx_t *cfgx, int8_t val)
{
  AVB_RC_t rc = AVB_SUCCESS;

  if (cfgx->valid != AVB_TRUE)
  {
    CFGX_TRACE_ERR(CFGX_TRACE_ERR_FMT_UNINITIALIZED_ELEMENT, __FUNCTION__, cfgx->key);
    rc = AVB_FAILURE;
  }
  else if ((val < cfgx->min) || (val > cfgx->max))
  {
    rc = AVB_FAILURE;
  }
  else
  {
    cfgx->val = val;
  }
  return rc;
}

void cfgxEnum8Len16Init(enum8_len16_cfgx_t *cfgx, uint16_t key, uint8_t val, uint8_t *enumList, uint8_t len, uint8_t val_default, uint8_t flags)
{
  int       i;
  uint8_t   valIsValid = AVB_FALSE;

  sal_memset(cfgx, 0, sizeof(enum8_len16_cfgx_t));
  if (len > CFGX_ENUM8_LEN16)
  {
    CFGX_TRACE_ERR("Error: This enum can not support more than %d entries", CFGX_ENUM8_LEN16);
    cfgx->valid = AVB_FALSE;
    return;
  }
  for (i=0; i<len; i++)
  {
    if (enumList[i] == val)
    {
      valIsValid = AVB_TRUE;
    }
    cfgx->enumList[i] = enumList[i];
  }
  if (valIsValid == AVB_FALSE)
  {
    CFGX_TRACE_WARN("Warning: Value out of range for key 0x%04x, initializing to default", key);
    val = val_default;
  }
  cfgx->key = key;
  cfgx->val = val;
  cfgx->val_boot = val;
  cfgx->val_default = val_default;
  cfgx->len = len;
  cfgx->flags = flags;
  cfgx->valid = AVB_TRUE;
}

AVB_RC_t cfgxEnum8Len16Set(enum8_len16_cfgx_t *cfgx, uint8_t val)
{
  AVB_RC_t rc = AVB_FAILURE;
  uint8_t  i;

  if (cfgx->valid != AVB_TRUE)
  {
    CFGX_TRACE_ERR(CFGX_TRACE_ERR_FMT_UNINITIALIZED_ELEMENT, __FUNCTION__, cfgx->key);
  }
  else
  {
    for (i=0; i<cfgx->len; i++)
    {
      if (val == cfgx->enumList[i])
      {
        rc = AVB_SUCCESS;
        cfgx->val = val;
        break;
      }
    }
  }
  return rc;
}


void cfgxUint16Init(uint16_cfgx_t *cfgx, uint16_t key, uint16_t val, uint16_t min, uint16_t max, uint16_t val_default, uint8_t flags)
{
  if ((flags & CFGX_FLAG_INIT_NOCHECK) != CFGX_FLAG_INIT_NOCHECK) /* Give the option to turn off checking for initialization */
  {
    if ((val < min) || (val > max))
    {
      CFGX_TRACE_WARN("Warning: Value out of range for key 0x%04x, initializing to default", key);
      val = val_default;
    }
  }
  cfgx->key = key;
  cfgx->val = val;
  cfgx->val_boot = val;
  cfgx->val_default = val_default;
  cfgx->min = min;
  cfgx->max = max;
  cfgx->flags = flags;
  cfgx->valid = AVB_TRUE;
}

AVB_RC_t cfgxUint16Set(uint16_cfgx_t *cfgx, uint16_t val)
{
  AVB_RC_t rc = AVB_SUCCESS;

  if (cfgx->valid != AVB_TRUE)
  {
    CFGX_TRACE_ERR(CFGX_TRACE_ERR_FMT_UNINITIALIZED_ELEMENT, __FUNCTION__, cfgx->key);
    rc = AVB_FAILURE;
  }
  else if ((val < cfgx->min) || (val > cfgx->max))
  {
    rc = AVB_FAILURE;
  }
  else
  {
    cfgx->val = val;
  }
  return rc;
}

void cfgxUint16SetZero(uint16_cfgx_t *cfgx)
{
  cfgx->val = 0;
}


void cfgxUint32Init(uint32_cfgx_t *cfgx, uint16_t key, uint32_t val, uint32_t min, uint32_t max, uint32_t val_default, uint8_t flags)
{
  if ((flags & CFGX_FLAG_INIT_NOCHECK) != CFGX_FLAG_INIT_NOCHECK) /* Give the option to turn off checking for initialization */
  {
    if ((val < min) || (val > max))
    {
      CFGX_TRACE_WARN("Warning: Value out of range for key 0x%04x, initializing to default", key);
      val = val_default;
    }
  }
  cfgx->key = key;
  cfgx->val = val;
  cfgx->val_boot = val;
  cfgx->val_default = val_default;
  cfgx->min = min;
  cfgx->max = max;
  cfgx->flags = flags;
  cfgx->valid = AVB_TRUE;
}

AVB_RC_t cfgxUint32Set(uint32_cfgx_t *cfgx, uint32_t val)
{
  AVB_RC_t rc = AVB_SUCCESS;

  if (cfgx->valid != AVB_TRUE)
  {
    CFGX_TRACE_ERR(CFGX_TRACE_ERR_FMT_UNINITIALIZED_ELEMENT, __FUNCTION__, cfgx->key);
    rc = AVB_FAILURE;
  }
  else if ((val < cfgx->min) || (val > cfgx->max))
  {
    rc = AVB_FAILURE;
  }
  else
  {
    cfgx->val = val;
  }
  return rc;
}

