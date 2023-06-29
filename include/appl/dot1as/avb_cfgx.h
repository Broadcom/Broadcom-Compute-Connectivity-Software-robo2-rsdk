/*
 * $Id$
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:    avb_cfgx.h
 */

#ifndef AVB_CFGX_H
#define AVB_CFGX_H

#include "avb.h"

/* Trace Flags */
#define CFGX_TRACE_DBG_FLAG            0x1   /* Debug traces like func calls */
#define CFGX_TRACE_ERR_FLAG            0x2   /* Errors */
#define CFGX_TRACE_WARN_FLAG           0x4   /* Warnings */
#define CFGX_TRACE_PARSE_WARN_FLAG     0x8   /* JSON Parser Warnings */


/* Debug flag and macro */
extern uint32_t cfgxTraceMask;

/* Show this output unconditionally */
#define CFGX_TRACE(format, args...)                                 \
 do                                                                 \
 {                                                                  \
   sal_printf(format, ##args);                                      \
 } while(0)

#define CFGX_TRACE_DBG(format, args...)                             \
 do                                                                 \
 {                                                                  \
   if ((cfgxTraceMask & CFGX_TRACE_DBG_FLAG))                       \
   {                                                                \
     sal_printf(format"\n", ##args);                                \
   }                                                                \
 } while(0)

#define CFGX_TRACE_ERR_FMT_UNINITIALIZED_ELEMENT  "Error: (%s) Trying to set configuration for an uninitialized element 0x%04x, nothing done"
#define CFGX_TRACE_ERR(format, args...)                             \
 do                                                                 \
 {                                                                  \
   if ((cfgxTraceMask & CFGX_TRACE_ERR_FLAG))                       \
   {                                                                \
     sal_printf(format"\n", ##args);                                \
   }                                                                \
 } while(0)

#define CFGX_TRACE_WARN(format, args...)                            \
 do                                                                 \
 {                                                                  \
   if ((cfgxTraceMask & CFGX_TRACE_WARN_FLAG))                      \
   {                                                                \
     sal_printf(format"\n", ##args);                                \
   }                                                                \
 } while(0)

#define CFGX_TRACE_PARSE_WARN(format, args...)                      \
 do                                                                 \
 {                                                                  \
   if ((cfgxTraceMask & CFGX_TRACE_PARSE_WARN_FLAG))                \
   {                                                                \
     sal_printf(format"\n", ##args);                                \
   }                                                                \
 } while(0)

#define CFGX_PRINT_LINE(n)                            \
do {                                                  \
  uint32_t i;                                         \
  for (i=0; i < (n); i++) { sal_printf("-"); }        \
  sal_printf("\n");                                   \
} while(0)


#define CLOCK_ID_LEN   8 /* Length of clock identify - 8 octets */

#define CFG_HEADER_KEY_STR                    "header"
#define CFG_HEADER_HDR_VER_KEY_STR            "hdrVer"
#define CFG_HEADER_COMP_ID_KEY_STR            "compId"
#define CFG_HEADER_COMP_VER_KEY_STR           "compVer"

#define CFGX_COMPONENT_HDR_VER       10

#define CFGX_COMPONENT_STR_END       "compCfgEnd"

#define CFGX_COMPONENT_STR_DOT1AS    "dot1asCfg"
#define CFGX_COMPONENT_LEN            7

#define INTF_CFG_KEY_STR                      "intfCfg"
#define INTF_KEY_STR                          "intf"
#define MODE_KEY_STR                          "mode"

#define CFGX_FLAG_MACADDR         0x01
#define CFGX_FLAG_HEX             0x02
#define CFGX_FLAG_PORT            0x04
#define CFGX_FLAG_INIT_NOCHECK    0x08 /* Allow initialization using an out of range value */

#define CFGX_TYPE_HEADER     1
#define CFGX_TYPE_UINT8      2
#define CFGX_TYPE_INT8       3
#define CFGX_TYPE_UINT16     4
#define CFGX_TYPE_UINT32     5
#define CFGX_TYPE_BOOL       6
#define CFGX_TYPE_ENUM8LEN16 7
#define CFGX_TYPE_CLOCKID    8
#define CFGX_TYPE_MACADDR    9
#define CFGX_TYPE_INTF      10

extern uint8_t *avbCfgxIntfNum;

typedef struct cfgx_comp_hdr_s {
  uint16_t   hdrVer;                  /* component header version */
  uint16_t   compId;                  /* component id*/
  uint16_t   compVer;                 /* component software version */
} cfgx_comp_hdr_t;

typedef struct intf_cfgx_s {
  uint16_t    key;
  uint8_t     valid;
  uint8_t     flags;         /* HEX, DECIMAL, other */
  uint16_t    type;     /* TRGTRG TODO */
  uint16_t    val;
  uint16_t    val_default;
  uint16_t    val_boot;
} intf_cfgx_t;


typedef struct bool_cfgx_s {
  uint16_t    key;
  uint8_t     valid;
  uint8_t     val;
  uint8_t     val_default;
  uint8_t     val_boot;
} bool_cfgx_t;

typedef struct uint8_cfgx_s {
  uint16_t    key;
  uint8_t     valid;
  uint8_t     flags;         /* HEX, DECIMAL, other */
  uint8_t     val;
  uint8_t     min;
  uint8_t     max;
  uint8_t     val_default;
  uint8_t     val_boot;
} uint8_cfgx_t;

typedef struct int8_cfgx_s {
  uint16_t    key;
  uint8_t     valid;
  uint8_t     flags;         /* HEX, DECIMAL, other */
  int8_t      val;
  int8_t      min;
  int8_t      max;
  int8_t      val_default;
  int8_t      val_boot;
} int8_cfgx_t;

#define CFGX_ENUM8_LEN16 16
typedef struct enum8_len16_cfgx_s {
  uint16_t    key;
  uint8_t     valid;
  uint8_t     flags;         /* HEX, DECIMAL, other */
  uint8_t     val;
  uint8_t     enumList[CFGX_ENUM8_LEN16];
  uint8_t     len;
  uint8_t     val_default;
  uint8_t     val_boot;
} enum8_len16_cfgx_t;

typedef struct uint16_cfgx_s {
  uint16_t    key;
  uint8_t     valid;
  uint8_t     flags;         /* HEX, DECIMAL, other */
  uint16_t    val;
  uint16_t    min;
  uint16_t    max;
  uint16_t    val_default;
  uint16_t    val_boot;
} uint16_cfgx_t;

typedef struct uint32_cfgx_s {
  uint16_t    key;
  uint8_t     valid;
  uint8_t     flags;         /* HEX, DECIMAL, other */
  uint32_t    val;
  uint32_t    min;
  uint32_t    max;
  uint32_t    val_default;
  uint32_t    val_boot;
} uint32_cfgx_t;

typedef struct int32_cfgx_s {
  uint16_t    key;
  uint8_t     valid;
  uint8_t     flags;         /* HEX, DECIMAL, other */
  int32_t     val;
  int32_t     min;
  int32_t     max;
  int32_t     val_default;
  int32_t     val_boot;
} int32_cfgx_t;

extern AVB_RC_t cfgxIntfIndexGet(uint16_t intfNum, uint8_t *intfIndex);
extern void     cfgxIntfInit(intf_cfgx_t *cfgx, uint16_t key, uint8_t type, uint16_t val, uint16_t val_default, uint8_t flags);
extern AVB_RC_t cfgxIntfSet(intf_cfgx_t *cfgx, uint16_t val);
extern void     cfgxBoolInit(bool_cfgx_t *cfgx, uint16_t key, uint8_t val, uint8_t val_default);
extern AVB_RC_t cfgxBoolSet(bool_cfgx_t *cfgx, uint8_t val);
extern void     cfgxUint8Init(uint8_cfgx_t *cfgx, uint16_t key, uint8_t val, uint8_t min, uint8_t max, uint8_t val_default, uint8_t flags);
extern AVB_RC_t cfgxUint8Set(uint8_cfgx_t *cfgx, uint8_t val);
extern void     cfgxInt8Init(int8_cfgx_t *cfgx, uint16_t key, int8_t val, int8_t min, int8_t max, int8_t val_default, uint8_t flags);
extern AVB_RC_t cfgxInt8Set(int8_cfgx_t *cfgx, int8_t val);
extern void     cfgxEnum8Len16Init(enum8_len16_cfgx_t *cfgx, uint16_t key, uint8_t val, uint8_t *enumList, uint8_t len, uint8_t val_default, uint8_t flags);
extern AVB_RC_t cfgxEnum8Len16Set(enum8_len16_cfgx_t *cfgx, uint8_t val);
extern void     cfgxUint16Init(uint16_cfgx_t *cfgx, uint16_t key, uint16_t val, uint16_t min, uint16_t max, uint16_t val_default, uint8_t flags);
extern AVB_RC_t cfgxUint16Set(uint16_cfgx_t *cfgx, uint16_t val);
extern void     cfgxUint16SetZero(uint16_cfgx_t *cfgx);
extern void     cfgxUint32Init(uint32_cfgx_t *cfgx, uint16_t key, uint32_t val, uint32_t min, uint32_t max, uint32_t val_default, uint8_t flags);
extern AVB_RC_t cfgxUint32Set(uint32_cfgx_t *cfgx, uint32_t val);
extern void     cfgxDump(uint8_t cfgxType, void *cfgxVoid, char *keyStr, uint16_t key);

#endif /* AVB_CFGX_H */
