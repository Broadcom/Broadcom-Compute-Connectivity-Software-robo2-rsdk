/* * $ID: $
 * * $Copyright: (c) 2017 Broadcom Corp.
 * * All Rights Reserved$
 * *
 * * File:
 * *     app.h
 * *
 * */

#ifndef APP_H
#define APP_H

#include "frozen.h"

#define APP_JSON_BUFSZ 2048  // Use a buffer to chunk out large JSON responses

#define LOGIN_PASSWORD_DEFAULT "password"

#define APP_TRUE   1
#define APP_FALSE  0

#define APP_ENABLE   1
#define APP_DISABLE  0

#define MAX_POSSIBLE_PHYS_PORTS     32 // TRGTRG-TODO fix this hack 
#define PHYS_PORT_USER_FIRST        1  // User facing starting port number
#define LAG_PORT_USER_FIRST         1  // User facing starting lag number

#define APP_IPADDR_BYTES_MIN    4

/* TRGTRGTRG  - TODO 
   TRGTRGTRG    Consider multiple log macros
   TRGTRGTRG    so we can enable some levels without affecting critical
   TRGTRGTRG    code paths. Maybe just add a special macro for those.
   TRGTRGTRG    
*/

#define APP_LOG_ALWAYS   1
#define APP_LOG_ERROR    1
#define APP_LOG_WARN     2
#define APP_LOG_DEBUG    3

void app_log_printf(const char *format, ...);

#define APP_LOG_LEVEL    APP_LOG_WARN
#ifdef APP_LOG_LEVEL
  #define APP_LOG(l, x)                               \
    if (APP_LOG_LEVEL >= l) {                         \
      sal_printf("%s:%d ",__FUNCTION__, __LINE__);    \
      app_log_printf x;                               \
      sal_putchar('\n');                              \
    }
#else
  #define APP_LOG(l, x) 
#endif


#define APP_TRACE_PORT_OFF
#ifdef APP_TRACE_PORT_ON
  #define APP_TRACE_PORT(x)                         \
      sal_printf("%s:%d ",__FUNCTION__, __LINE__);  \
      app_log_printf x;                             \
      sal_putchar('\n');
#else
  #define APP_TRACE_PORT(x) 
#endif

#define APP_TRACE_VLAN_OFF
#ifdef APP_TRACE_VLAN_ON
  #define APP_TRACE_VLAN(x)                         \
      sal_printf("%s:%d ",__FUNCTION__, __LINE__);  \
      app_log_printf x;                             \
      sal_putchar('\n');
#else
  #define APP_TRACE_VLAN(x) 
#endif


#define APP_TRACE_CFG_OFF
#ifdef APP_TRACE_CFG_ON
  #define APP_TRACE_CFG(x)                         \
      sal_printf("%s:%d ",__FUNCTION__, __LINE__);  \
      app_log_printf x;                             \
      sal_putchar('\n');
#else
  #define APP_TRACE_CFG(x) 
#endif

#define APP_TRACE_L2_OFF
#ifdef APP_TRACE_L2_ON
  #define APP_TRACE_L2(x)                         \
      sal_printf("%s:%d ",__FUNCTION__, __LINE__);  \
      app_log_printf x;                             \
      sal_putchar('\n');
#else
  #define APP_TRACE_L2(x) 
#endif

#define APP_TRACE_SESSION_OFF
#ifdef APP_TRACE_SESSION_ON
  #define APP_TRACE_SESSION(x)                         \
      sal_printf("%s:%d ",__FUNCTION__, __LINE__);  \
      app_log_printf x;                             \
      sal_putchar('\n');
#else
  #define APP_TRACE_SESSION(x) 
#endif

#define APP_TRACE_FW_UPGRADE_OFF
#ifdef APP_TRACE_FW_UPGRADE_ON
  #define APP_TRACE_FW_UPGRADE(x)                         \
      sal_printf("%s:%d ",__FUNCTION__, __LINE__);  \
      app_log_printf x;                             \
      sal_putchar('\n');
#else
  #define APP_TRACE_FW_UPGRADE(x) 
#endif

#define APP_TRACE_LAG_OFF
#ifdef APP_TRACE_LAG_ON
  #define APP_TRACE_LAG(x)                          \
      sal_printf("%s:%d ",__FUNCTION__, __LINE__);  \
      app_log_printf x;                             \
      sal_putchar('\n');
#else
  #define APP_TRACE_LAG(x)
#endif

#ifdef APP_TRACE_MIRROR_ON
  #define APP_TRACE_MIRROR(x)                          \
      sal_printf("%s:%d ",__FUNCTION__, __LINE__);  \
      app_log_printf x;                             \
      sal_putchar('\n');
#else
  #define APP_TRACE_MIRROR(x)
#endif

#ifdef APP_TRACE_DOS_ON
  #define APP_TRACE_DOS(x)                          \
      sal_printf("%s:%d ",__FUNCTION__, __LINE__);  \
      app_log_printf x;                             \
      sal_putchar('\n');
#else
  #define APP_TRACE_DOS(x)
#endif

#ifdef APP_TRACE_STORM_ON
  #define APP_TRACE_STORM(x)                        \
      sal_printf("%s:%d ",__FUNCTION__, __LINE__);  \
      app_log_printf x;                             \
      sal_putchar('\n');
#else
  #define APP_TRACE_STORM(x)
#endif

#define CONTEXT_QSTR_MAX    64
#define CONTEXT_ERRMSG_MAX  128

#define OFFSET_OF(TYPE, FIELD) (long)&((TYPE *)0)->FIELD

#define APP_IS_MASK_BIT_SET(intf_mask, intf) (intf_mask & (1ULL << intf))
#define APP_MASK_BIT_CLR(intf_mask, intf) (intf_mask &= ~(1ULL << intf))
#define APP_MASK_BIT_SET(intf_mask, intf) (intf_mask |= (1ULL << intf))


/* Memory buffer descriptor
 * This definition should match the 
 * definition for struct mbuf in mongoose.h 
 */
struct app_mbuf {
  char *buf;   /* Buffer pointer */
  size_t len;  /* Data length. Data is located between offset 0 and len. */
  size_t size; /* Buffer size allocated by realloc(1). Must be >= len */
};

void get_app_mbuf_offsets(int *buf_offset, int *len_offset, int *size_offset);

#define JSON_OUT_MBUF(app_mbuf_addr) \
  { .printer = app_json_printer_mbuf, .u.data = (void *) app_mbuf_addr }


#define WEB_ENABLE   1
#define WEB_DISABLE  0

// These error codes/messages correspond to _SHR_E_xxxx in include/shared/error.h
typedef enum {
    APP_E_NONE                 =   0,
    APP_E_INTERNAL             =  -1,
    APP_E_MEMORY               =  -2,
    APP_E_UNIT                 =  -3,
    APP_E_PARAM                =  -4,
    APP_E_EMPTY                =  -5,
    APP_E_FULL                 =  -6,
    APP_E_NOT_FOUND            =  -7,
    APP_E_EXISTS               =  -8,
    APP_E_TIMEOUT              =  -9,
    APP_E_BUSY                 = -10,
    APP_E_FAIL                 = -11,
    APP_E_DISABLED             = -12,
    APP_E_BADID                = -13,
    APP_E_RESOURCE             = -14,
    APP_E_CONFIG               = -15,
    APP_E_UNAVAIL              = -16,
    APP_E_INIT                 = -17,
    APP_E_PORT                 = -18,
    APP_E_UNKNOWN              = -19,     /* Up to here must match _SHR_E_xxxx */ 
    APP_E_CFG_READ             = -20,
    APP_E_CFG_WRITE            = -21,
    APP_E_AUTH                 = -22,

    APP_E_LIMIT                = -23           /* Must come last */
} APP_RC_t;

#define APP_E_UNKNOWN      APP_E_LIMIT

#define APP_ERRMSG_INIT        { \
        "Ok",                           /* E_NONE */ \
        "Internal error",               /* E_INTERNAL */ \
        "Out of memory",                /* E_MEMORY */ \
        "Invalid unit",                 /* E_UNIT */ \
        "Invalid parameter",            /* E_PARAM */ \
        "Table empty",                  /* E_EMPTY */ \
        "Table full",                   /* E_FULL */ \
        "Entry not found",              /* E_NOT_FOUND */ \
        "Entry exists",                 /* E_EXISTS */ \
        "Operation timed out",          /* E_TIMEOUT */ \
        "Operation still running",      /* E_BUSY */ \
        "Operation failed",             /* E_FAIL */ \
        "Operation disabled",           /* E_DISABLED */ \
        "Invalid identifier",           /* E_BADID */ \
        "No resources for operation",   /* E_RESOURCE */ \
        "Invalid configuration",        /* E_CONFIG */ \
        "Feature unavailable",          /* E_UNAVAIL */ \
        "Feature not initialized",      /* E_INIT */ \
        "Invalid port",                 /* E_PORT */ \
        "Unknown error",                /* E_LIMIT */ \
        "Configuration read error",     /* E_WEB_CFG_READ */ \
        "Configuration write error"    /* E_WEB_CFG_WRITE */ \
        }

extern char *app_errmsg[];

#define APP_ERRMSG(r)          \
        app_errmsg[(((int)r) <= 0 && ((int)r) > APP_E_LIMIT) ? -(r) : -APP_E_LIMIT]

#define APP_SUCCESS(rv)    ((rv) >= 0)
#define APP_FAILURE(rv)    ((rv) < 0)


char    *app_inet_ntoa(uint32_t addr, char *ip, uint8_t iplen);
int app_json_printer_mbuf(struct json_out *out, const char *buf, size_t len);

#endif /* APP_H */
