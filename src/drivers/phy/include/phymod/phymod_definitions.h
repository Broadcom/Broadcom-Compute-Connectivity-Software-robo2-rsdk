/*
 *         
 * $Id: phymod_definitions.h,v 1.2.2.12 2013/09/03 06:54:51 dayad Exp $
 * 
 * $Copyright: 
 * All Rights Reserved.$
 *         
 *     
 * Phymod external definitions
 *
 */

/* FIXME: need a better name for this file */

#ifndef _PHYMOD_DEFINITIONS_H_
#define _PHYMOD_DEFINITIONS_H_

#include <phymod/phymod_system.h>

#define _PHYMOD_MSG(string) "%s[%d]%s: " string "\n", __FILE__, __LINE__, FUNCTION_NAME()


#ifdef PHYMOD_DIAG 
typedef struct enum_mapping_s {
    char        *key;           /* String to match */
    uint32_t    enum_value;     /* Value associated with string */
} enum_mapping_t;
#endif


/* FIXME: should be cleaned up everywhere */
#define PHYMOD_E_OK PHYMOD_E_NONE

#define PHYMOD_LOCK_TAKE(t) do{\
        if((t->access.bus->mutex_give != NULL) && (t->access.bus->mutex_take != NULL)){\
            PHYMOD_IF_ERR_RETURN(t->access.bus->mutex_take(t->access.user_acc));\
        }\
    }while(0)
 
#define PHYMOD_LOCK_GIVE(t) do{\
        if((t->access.bus->mutex_give != NULL) && (t->access.bus->mutex_take != NULL)){\
            PHYMOD_IF_ERR_RETURN(t->access.bus->mutex_give(t->access.user_acc));\
        }\
    }while(0)

/* Dispatch */
#define PHYMOD_DRIVER_TYPE_GET(p,t) ((*t) = p->type)

/* Functions structure */
#define PHYMOD_IF_ERR_RETURN(A) \
    do {   \
        int loc_err ; \
        if ((loc_err = (A)) != PHYMOD_E_NONE) \
        {  return loc_err ; } \
    } while (0)

#define PHYMOD_RETURN_WITH_ERR(A, B) \
    do {   \
        PHYMOD_DEBUG_ERROR(B);     \
        return A; \
    } while (0)

#define PHYMOD_NULL_CHECK(A) \
    do {   \
        if ((A) == NULL) \
        { PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("null parameter"))); } \
    } while (0)

#define PHYMOD_ERR_VALUE_CHECK(A, VAL) \
    do {   \
        if ((A) == VAL) \
        { PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("invalid value"))); } \
    } while (0)

#endif /*_PHYMOD_DEFINITIONS_H_*/
