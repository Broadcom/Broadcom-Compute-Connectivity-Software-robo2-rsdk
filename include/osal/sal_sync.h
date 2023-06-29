/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File: sal_sync.h
 * Purpose: SAL synchronization definitions
 *
 */

#ifndef _SAL_SYNC_H
#define _SAL_SYNC_H

#ifdef OPENRTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#include "sal_list.h"
#include "sal_alloc.h"
#include "sal_console.h"
#include "bsl.h"

#ifndef OPENRTOS
typedef int32_t xSemaphoreHandle;
#endif


#ifdef OSAL_SYNC_MEM_DEBUG
typedef struct _sal_mutex_hdl_t {
    node_t    node;
    char      name[configMAX_TASK_NAME_LEN];
    xSemaphoreHandle hdl;
} sal_mutex_hdl_t;

typedef sal_mutex_hdl_t*    sal_mutex_t;

typedef struct _sal_sem_hdl_t {
    node_t  node;
    int8_t    name[configMAX_TASK_NAME_LEN];
    xSemaphoreHandle hdl;
    int32_t   init_count;
} sal_sem_hdl_t;

typedef sal_sem_hdl_t*    sal_sem_t;
#else
/* TBD fix this to typedef to SemaphoreHandle_t */
typedef void * sal_mutex_t;
typedef void * sal_sem_t;
#endif  /* OSAL_SYNC_MEM_DEBUG */

#define SAL_MAX_SEM_COUNT    (64)
#define sal_mutex_FOREVER    (-1)
#define sal_sem_FOREVER      (-1)
extern sal_mutex_t sal_mutex_create(char *desc);
extern void sal_mutex_destroy(sal_mutex_t mtex);
extern int sal_mutex_take(sal_mutex_t mtex, int usec);
extern int sal_mutex_give(sal_mutex_t mtex);

extern sal_sem_t sal_sem_create(char *desc, int binary, int init_count);
extern void sal_sem_destroy(sal_sem_t sem);
extern int sal_sem_take(sal_sem_t sem, int usec);
extern int sal_sem_give(sal_sem_t sem);


#endif	/* !_SAL_SYNC_H */
