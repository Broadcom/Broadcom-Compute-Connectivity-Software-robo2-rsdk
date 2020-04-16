/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * * All Rights Reserved$
 * *
 * * File:
 * *     sal_thread.c
 * * Purpose:	Defines SAL routines for linux threads
 * *
 * * Thread Abstraction
 * *
 * *
 */

#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
//#include <sys/prctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sal_console.h>
#include <sal_task.h>
#include <sal_sync.h>
#include <sal_time.h>

#if defined (__STRICT_ANSI__)
#define NO_CONTROL_C
#endif

#ifndef SAL_THREAD_RT_PRIO_HIGHEST
#define SAL_THREAD_RT_PRIO_HIGHEST  90
#endif

static pthread_mutex_t _sal_thread_lock = PTHREAD_MUTEX_INITIALIZER;

#define THREAD_LOCK() pthread_mutex_lock(&_sal_thread_lock)
#define THREAD_UNLOCK() pthread_mutex_unlock(&_sal_thread_lock)


#ifndef PTHREAD_STACK_MIN
#  ifdef PTHREAD_STACK_SIZE
#    define PTHREAD_STACK_MIN PTHREAD_STACK_SIZE
#  else
#    define PTHREAD_STACK_MIN 0
#  endif
#endif

/*
 * Function:
 *	thread_boot
 * Purpose:
 *	Entry point for each new thread created
 * Parameters:
 *	ti - information about thread being created
 * Notes:
 *	Signals and other parameters are configured before jumping to
 *	the actual thread's main routine.
 */

typedef struct thread_info_s {
    void		(*f)(void *);
    char		*name;
    pthread_t		id;
    void		*arg;
    int                 ss;
    sal_sem_t           sem;
    struct thread_info_s *next;
} thread_info_t;

static thread_info_t	*thread_head = NULL;

static void *
thread_boot(void *ti_void)
{

    thread_info_t	*ti = ti_void;
    void		(*f)(void *);
    void		*arg;
#ifndef NO_CONTROL_C
    sigset_t		new_mask, orig_mask;

    /* Make sure no child thread catches Control-C */
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &new_mask, &orig_mask);
#endif

    /* Ensure that we give up all resources upon exit */
    pthread_detach(pthread_self());

#ifdef PR_SET_NAME
    prctl(PR_SET_NAME, ti->name, 0, 0, 0);
#endif

#ifndef netbsd
    /* not supported */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
#endif /* netbsd */

    f = ti->f;
    arg = ti->arg;

    ti->id = pthread_self();

    /* Notify parent to continue */
    sal_sem_give(ti->sem);

    /* Call thread function */
    (*f)(arg);

    /* Thread function did not call sal_thread_exit() */
    sal_thread_exit(0);

    /* Will never get here */
    return NULL;
}

/*
 * Function:
 *	sal_thread_create
 * Purpose:
 *	Abstraction for task creation
 * Parameters:
 *	name - name of task
 *	ss - stack size requested
 *	prio - scheduling prio (0 = highest, 255 = lowest)
 *	func - address of function to call
 *	arg - argument passed to func.
 * Returns:
 *	Thread ID
 */

sal_thread_t
sal_thread_create(char *name, int ss, int prio, void (f)(void *), void *arg)
{
    pthread_attr_t	attribs;
    struct sched_param param;
    thread_info_t	*ti;
    pthread_t		id;
    sal_sem_t           sem;

    if (pthread_attr_init(&attribs)) {
        return(SAL_THREAD_ERROR);
    }

    ss += PTHREAD_STACK_MIN;
    pthread_attr_setstacksize(&attribs, ss);

    if (prio == SAL_THREAD_PRIO_NO_PREEMPT) {
        pthread_attr_setinheritsched(&attribs, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attribs, SCHED_FIFO);
        param.sched_priority = SAL_THREAD_RT_PRIO_HIGHEST;
        pthread_attr_setschedparam(&attribs, &param);
    } else if (prio == SAL_THREAD_RT_PRIO_HIGHEST) {
        /* For real time thread priorities use SCHED_RR. Needs to be changed
         * to make generic to work for all real time priorities */
        pthread_attr_setinheritsched(&attribs, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attribs, SCHED_RR);
        param.sched_priority = SAL_THREAD_RT_PRIO_HIGHEST;
        pthread_attr_setschedparam(&attribs, &param);
    }

    if ((ti = malloc(sizeof (*ti))) == NULL) {
	return SAL_THREAD_ERROR;
    }

    if ((sem = sal_sem_create("threadBoot", 1, 0)) == NULL) {
	free(ti);
	return SAL_THREAD_ERROR;
    }
    ti->name = NULL;
    if ((ti->name = malloc(strlen(name)+1)) == NULL) {
        free(ti);
        sal_sem_destroy(sem);
        return SAL_THREAD_ERROR;
    }
    /* coverity[secure_coding] */
    strcpy(ti->name, name);

    ti->f = f;

    ti->arg = arg;
    ti->id = (pthread_t)0;
    ti->ss = ss;
    ti->sem = sem;

    THREAD_LOCK();
    ti->next = thread_head;
    thread_head = ti;
    THREAD_UNLOCK();

    if (pthread_create(&id, &attribs, thread_boot, (void *)ti)) {
        THREAD_LOCK();
	thread_head = thread_head->next;
        THREAD_UNLOCK();
        if (ti->name != NULL) {
            free(ti->name);
        }
	free(ti);
        sal_sem_destroy(sem);
	return(SAL_THREAD_ERROR);
    }


    /*
     * Note that at this point ti can no longer be safely
     * dereferenced, as the thread we just created may have
     * exited already. Instead we wait for the new thread
     * to update thread_info_t and tell us to continue.
     */
    sal_sem_take(sem, sal_sem_FOREVER);
    sal_sem_destroy(sem);

    return ((sal_thread_t)id);
}

/*
 * Function:
 *	sal_thread_destroy
 * Purpose:
 *	Abstraction for task deletion
 * Parameters:
 *	thread - thread ID
 * Returns:
 *	0 on success, -1 on failure
 * Notes:
 *	This routine is not generally used by Broadcom drivers because
 *	it's unsafe.  If a task is destroyed while holding a mutex or
 *	other resource, system operation becomes unpredictable.  Also,
 *	some RTOS's do not include kill routines.
 *
 *	Instead, Broadcom tasks are written so they can be notified via
 *	semaphore when it is time to exit, at which time they call
 *	sal_thread_exit().
 */

int
sal_thread_destroy(sal_thread_t thread)
{
#ifdef netbsd
    /* not supported */
    return -1;
#else
    thread_info_t	*ti, **tp;
    pthread_t		id = (pthread_t) thread;

    if (pthread_cancel(id)) {
	return -1;
    }

    ti = NULL;

    THREAD_LOCK();
    for (tp = &thread_head; (*tp) != NULL; tp = &(*tp)->next) {
	if ((*tp)->id == id) {
	    ti = (*tp);
	    (*tp) = (*tp)->next;
	    break;
	}
    }
    THREAD_UNLOCK();

    if (ti) {
        if (ti->name != NULL) {
            free(ti->name);
        }
        free(ti);
    }

    return 0;
#endif
}

/*
 * Function:
 *	sal_thread_self
 * Purpose:
 *	Return thread ID of caller
 * Parameters:
 *	None
 * Returns:
 *	Thread ID
 */

sal_thread_t
sal_thread_self(void)
{
    return (sal_thread_t) pthread_self();
}


/*
 * Function:
 *	_sal_thread_name
 * Purpose:
 *	Return name given to thread when it was created
 * Parameters:
 *	thread - thread ID
 *	thread_name - buffer to return thread name;
 *		gets empty string if not available
 *	thread_name_size - maximum size of buffer
 * Returns:
 *	NULL, if name not available
 *	thread_name, if name available
 */
char *
_sal_thread_name(sal_thread_t thread, char *thread_name, int thread_name_size)
{
    thread_info_t	*ti;
    char                *name;

    name = NULL;

    THREAD_LOCK();
    for (ti = thread_head; ti != NULL; ti = ti->next) {
	if (ti->id == (pthread_t)thread) {
	    strncpy(thread_name, ti->name, thread_name_size);
	    thread_name[thread_name_size - 1] = 0;
	    name = thread_name;
            break;
	}
    }
    THREAD_UNLOCK();

    if (name == NULL) {
        thread_name[0] = 0;
    }

    return name;
}
/*
 * Function:
 *    sal_thread_name
 * Purpose:
 *    Return name given to thread when it was created.
 * Parameters:
 *    thread - thread ID
 * Returns:Returns Thread name on success. If the specified thread does not
 *         exist, NULL is returned.
 */
char* sal_thread_name(sal_thread_t thread)
{
	char thread_name[SAL_THREAD_NAME_MAX_LEN];
#ifdef OSAL_TASK_MEM_DEBUG
    if(thread == NULL)
#else
    if(thread == 0)
#endif
        return NULL;
    return _sal_thread_name(thread, thread_name, sizeof (thread_name));
}

/*
 * Function:
 *	sal_thread_exit
 * Purpose:
 *	Exit the calling thread
 * Parameters:
 *	rc - return code from thread.
 * Notes:
 *	Never returns.
 */

void
sal_thread_exit(int rc)
{
    thread_info_t	*ti, **tp;
    pthread_t		id = pthread_self();

    ti = NULL;

    THREAD_LOCK();
    for (tp = &thread_head; (*tp) != NULL; tp = &(*tp)->next) {
	if ((*tp)->id == id) {
	    ti = (*tp);
	    (*tp) = (*tp)->next;
	    break;
	}
    }
    THREAD_UNLOCK();

    if (ti) {
        if (ti->name != NULL) {
            free(ti->name);
        }
        free(ti);
    }

    pthread_exit(INT_TO_PTR(rc));
}


/*
 * Function:
 *	sal_thread_main_set
 * Purpose:
 *	Set which thread is the main thread
 * Parameters:
 *	thread - thread ID
 * Notes:
 *	The main thread is the one that runs in the foreground on the
 *	console.  It prints normally, takes keyboard signals, etc.
 */

static sal_thread_t _sal_thread_main = 0;

void
sal_thread_main_set(sal_thread_t thread)
{
    _sal_thread_main = thread;
}

/*
 * Function:
 *	sal_thread_main_get
 * Purpose:
 *	Return which thread is the main thread
 * Returns:
 *	Thread ID
 * Notes:
 *	See sal_thread_main_set().
 */

sal_thread_t
sal_thread_main_get(void)
{
    return _sal_thread_main;
}





