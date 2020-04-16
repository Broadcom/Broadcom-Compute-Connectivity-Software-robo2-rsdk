/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File: sal_task.h
 * Purpose: SAL thread definitions
 */

#ifndef _SAL_TASK_H
#define _SAL_TASK_H

#ifdef OPENRTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

#include "sal_list.h"
#include "sal_alloc.h"
#include "sal_console.h"
#include "bsl.h"

#ifndef OPENRTOS
typedef int32_t xTaskHandle;
#define configMAX_TASK_NAME_LEN 64
#endif

#ifdef OSAL_TASK_MEM_DEBUG
typedef struct _sal_task_t {
    node_t  node;
    int8_t    name[configMAX_TASK_NAME_LEN];
    void    (*entry)(void *);
    void    *param;
    int32_t   stack_size;
    int32_t   priority;
    xTaskHandle hdl;
} sal_task_t;

typedef sal_task_t *   sal_thread_t;

#else
typedef xTaskHandle   sal_thread_t;
#endif /* OSAL_TASK_MEM_DEBUG */
#define SAL_THREAD_ERROR    ((sal_thread_t) -1)
#ifndef SAL_THREAD_STKSZ
#define	SAL_THREAD_STKSZ	4096	/* Default Stack Size */
#endif
#define SAL_THREAD_NAME_MAX_LEN     80
#define SAL_THREAD_PRIO_NO_PREEMPT  -1

#define SAL_THREAD_DONE  0
#define SAL_THREAD_STOP  1
#define SAL_THREAD_RUNNING  2

#define DBG_MAX_THREADS     12
extern void* threads_g[DBG_MAX_THREADS];

extern sal_thread_t   sal_thread_create(char *name, int stack_size,
                                        int priority, void (f)(void *),  void *arg);

extern int sal_thread_destroy(sal_thread_t thread);
extern sal_thread_t sal_thread_self(void);
extern char* sal_thread_name(sal_thread_t thread);
extern void sal_thread_exit(int);
extern char *_sal_thread_name(sal_thread_t thread,
                              char *thread_name, int thread_name_size);
sal_thread_t sal_thread_main_get(void);
extern uint32 sal_thread_yeild(void);
extern void sal_thread_notify(sal_thread_t thread, uint32 value);


#endif	/* !_SAL_TASK_H */
