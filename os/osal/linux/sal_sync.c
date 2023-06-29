/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Corp.
 * * All Rights Reserved$
 * *
 * * File:
 * *     sal_sync.c
 * * Purpose: Defines sal routines for mutexes and semaphores
 * *
 * * Mutex and Binary Semaphore abstraction
 * *
 * * Note: the SAL mutex abstraction is required to allow the same mutex
 * * to be taken recursively by the same thread without deadlock.
 * *
 * * The POSIX mutex used here has been further abstracted to ensure this.
 * *
 * */


#if defined(LINUX) || defined(linux) || defined(__linux__)
#define _XOPEN_SOURCE 600
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>

#include <sal_console.h>
#include <sal_sync.h>
#include <sal_time.h>
#include <sal_libc.h>
#include <sal_task.h>

#if defined(LINUX) && defined(__USE_UNIX98)

#define USE_POSIX_RECURSIVE_MUTEX

#endif


#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS >= 200112L) && \
        defined(_POSIX_TIMEOUTS) && (_POSIX_TIMEOUTS >= 200112L)
/* Sometimes customers want to set new real time, so they must use monotic time.
  * But if we define USE_POSIX_SEM_TIMEDWAIT or USE_POSIX_MUTEX_TIMEDLOCK,
  * we will use real time, so we add this new Micro */
#define USE_POSIX_SEM_TIMEDWAIT
#define USE_POSIX_MUTEX_TIMEDLOCK
#endif


#if defined (__STRICT_ANSI__)
#define NO_CONTROL_C
#endif

#ifndef SECOND_NSEC
#define SECOND_NSEC     (SECOND_USEC * 1000)
#endif


/*
 * Keyboard interrupt protection
 *
 *   When a thread is running on a console, the user could Control-C
 *   while a mutex is held by the thread.  Control-C results in a signal
 *   that longjmp's somewhere else.  We prevent this from happening by
 *   blocking Control-C signals while any mutex is held.
 */

#ifndef NO_CONTROL_C
static int ctrl_c_depth = 0;
int ctrl_c_blocked = 0;
static sigset_t ctrl_c_sigset;
static int ctrl_c_initialized;
static sal_thread_t main_thread = SAL_THREAD_ERROR;
#endif

static void
ctrl_c_block(void)
{
#ifndef NO_CONTROL_C
    if (main_thread == SAL_THREAD_ERROR) {
        main_thread = sal_thread_main_get();
    }
    if (sal_thread_self() == main_thread) {
	if (ctrl_c_depth++ == 0) {
	    sigprocmask(SIG_BLOCK, &ctrl_c_sigset, NULL);
        ctrl_c_blocked = 1;
	}
    }
#endif    
}

static void
ctrl_c_unblock(void)
{
#ifndef NO_CONTROL_C
    if (main_thread == SAL_THREAD_ERROR) {
        main_thread = sal_thread_main_get();
    }
    if (sal_thread_self() == main_thread) {
	SAL_ASSERT(ctrl_c_depth > 0);
	if (--ctrl_c_depth == 0) {
	    sigprocmask(SIG_UNBLOCK, &ctrl_c_sigset, NULL);
        ctrl_c_blocked = 0;
	}
    }
#endif
}

#if defined(USE_POSIX_SEM_TIMEDWAIT) || defined(USE_POSIX_MUTEX_TIMEDLOCK)
static
int
_sal_compute_timeout(struct timespec *ts, int usec)
{
    int sec;
    uint32 nsecs;

#ifdef CLOCK_REALTIME
    if (clock_gettime(CLOCK_REALTIME, ts) == 0) {
        ;
    }
    else
#endif
    {
        struct timeval	ltv;

        /* Fall back to RTC if realtime clock unavailable */
        gettimeofday(&ltv, 0);
        ts->tv_sec = ltv.tv_sec;
        ts->tv_nsec = ltv.tv_usec * 1000;
    }
    /* Add in the delay */
    ts->tv_sec += usec / SECOND_USEC;

    /* compute new nsecs */
    nsecs = ts->tv_nsec + (usec % SECOND_USEC) * 1000;

    /* detect and handle rollover */
    if (nsecs < ts->tv_nsec) {
        ts->tv_sec += 1;
        nsecs -= SECOND_NSEC;
    }
    ts->tv_nsec = nsecs;

    /* Normalize if needed */
    sec = ts->tv_nsec / SECOND_NSEC;
    if (sec) {
        ts->tv_sec += sec;
        ts->tv_nsec = ts->tv_nsec % SECOND_NSEC;
    }

    /* indicate that we successfully got the time */
    return 1;
}
#endif

/*
 * recursive_mutex_t
 *
 *   This is an abstract type built on the POSIX mutex that allows a
 *   mutex to be taken recursively by the same thread without deadlock.
 *
 *   The Linux version of pthreads supports recursive mutexes
 *   (a non-portable extension to posix). In this case, we 
 *   use the Linux support instead of our own. 
 */

typedef struct recursive_mutex_s {
    pthread_mutex_t	mutex;
    char		*desc;
#ifndef USE_POSIX_RECURSIVE_MUTEX
    sal_thread_t	owner;
    int			recurse_count;
#endif


} recursive_mutex_t;


#ifdef netbsd
/*
 * The netbsd pthreads implementation we are using
 * does not seem to have his function
 */
static int
pthread_mutexattr_init(pthread_mutexattr_t* attr)
{
    attr->m_type = PTHREAD_MUTEXTYPE_DEBUG;
    attr->m_flags = 0;
    return 0;
}

#endif /* netbsd */



static sal_mutex_t
_sal_mutex_create(char *desc)
{
    recursive_mutex_t	*rm;
    pthread_mutexattr_t attr;


#ifndef NO_CONTROL_C
    if (!ctrl_c_initialized) {
        sigemptyset(&ctrl_c_sigset);
        sigaddset(&ctrl_c_sigset, SIGINT);
        ctrl_c_initialized = 1;
    }
#endif
    if ((rm = malloc(sizeof (recursive_mutex_t))) == NULL) {
	return NULL;
    }

    rm->desc = desc;
    pthread_mutexattr_init(&attr);
#ifdef USE_POSIX_RECURSIVE_MUTEX
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#else
    rm->owner = 0;
    rm->recurse_count = 0;

#endif

    pthread_mutex_init(&rm->mutex, &attr);

    return (sal_mutex_t) rm;
}

/*
 * Mutex and semaphore abstraction
 */

sal_mutex_t
sal_mutex_create(char *desc)
{
#ifdef SAL_GLOBAL_MUTEX
    static sal_mutex_t _m = NULL;
    if (!_m) {
	_m = _sal_mutex_create("sal_global_mutex");
	SAL_ASSERT(_m);
    }
    if (strcmp(desc, "spl mutex")) {
	return _m;
    }
#endif
    return _sal_mutex_create(desc);

}

void
sal_mutex_destroy(sal_mutex_t m)
{
    recursive_mutex_t	*rm = (recursive_mutex_t *) m;

    SAL_ASSERT(rm);

#ifndef USE_POSIX_RECURSIVE_MUTEX
    /* Check for pending mutex unlocks */
    if ((rm->recurse_count > 0) || (rm->owner != 0)) {
        char thread_name[SAL_THREAD_NAME_MAX_LEN];
        _sal_thread_name(rm->owner, thread_name, sizeof (thread_name));
        sal_printf("WARNING: Mutex \"%s\" has not been unlocked before being destroyed.\n",
                        rm->desc);
        sal_printf("\t Current owner is \"%s\" \n", thread_name);
    }
#ifdef SAL_MUTEX_DEBUG
    SAL_ASSERT(rm->recurse_count == 0);
    SAL_ASSERT(rm->owner == 0);
#endif /* SAL_MUTEX_DEBUG */


#endif /* !USE_POSIX_RECURSIVE_MUTEX */

    pthread_mutex_destroy(&rm->mutex);

    free(rm);

}

int
sal_mutex_take(sal_mutex_t m, int usec)
{
    recursive_mutex_t	*rm = (recursive_mutex_t *) m;
    int			err = 0;

#ifndef USE_POSIX_RECURSIVE_MUTEX
    sal_thread_t	myself = sal_thread_self();
#endif

#ifdef USE_POSIX_MUTEX_TIMEDLOCK
    struct timespec	ts;
#endif

    SAL_ASSERT(rm);

#ifndef USE_POSIX_RECURSIVE_MUTEX
    if (rm->owner == myself) {

        rm->recurse_count++;

        return 0;
    }

#endif


    ctrl_c_block();


    if (usec == sal_mutex_FOREVER) {
        do {
            err = pthread_mutex_lock(&rm->mutex);
        } while (err != 0 && errno == EINTR);
    }

#ifdef USE_POSIX_MUTEX_TIMEDLOCK
    else if (_sal_compute_timeout(&ts, usec)) {
        /* Treat EAGAIN as a fatal error on Linux */
        err = pthread_mutex_timedlock(&rm->mutex, &ts);
    }
#else
    else {
        int		time_wait = 1;

        /* Retry algorithm with exponential backoff */

        for (;;) {
            err = pthread_mutex_trylock(&rm->mutex);

            if (err != EBUSY) {
                break;		/* Done (0), or error other than EBUSY */
            }

            if (time_wait > usec) {
                time_wait = usec;
            }

            sal_usleep(time_wait);

            usec -= time_wait;

            if (usec == 0) {
                err = ETIMEDOUT;
                break;
            }

            if ((time_wait *= 2) > 100000) {
                time_wait = 100000;
            }
        }
    }
#endif

    if (err) {
        ctrl_c_unblock();

        SAL_ASSERT(usec != sal_mutex_FOREVER);
        return -1;
    }

#ifndef USE_POSIX_RECURSIVE_MUTEX
    SAL_ASSERT(rm->owner == 0);
    rm->owner = myself;
#endif
    return 0;
}



int
sal_mutex_give(sal_mutex_t m)
{
    recursive_mutex_t	*rm = (recursive_mutex_t *) m;
    int			err;

    SAL_ASSERT(rm);

#ifndef USE_POSIX_RECURSIVE_MUTEX
    if ((rm->owner != sal_thread_self())) {
        SAL_ASSERT(rm->owner == sal_thread_self());
    }

    if (rm->recurse_count > 0) {
        rm->recurse_count--;
        return 0;
    }

    rm->owner = 0;
#endif


    err = pthread_mutex_unlock(&rm->mutex);
    ctrl_c_unblock();

    SAL_ASSERT(err == 0);

    return err ? -1 : 0;
}


#ifndef LINUX_SAL_SEM_OVERRIDE

/*
 * Wrapper class to hold additional info
 * along with the semaphore.
 */
typedef struct {
    sem_t       s;
    char        *desc;
    int         binary;
} wrapped_sem_t;

sal_sem_t
sal_sem_create(char *desc, int binary, int initial_count)
{
    wrapped_sem_t *s = NULL;

    if ((s = malloc(sizeof (wrapped_sem_t))) == NULL) {
        return NULL;
    }

    /* 
     * This is needed by some libraries with a bug requiring to zero sem_t before calling sem_init(),
     * even though this it is not required by the function description.
     * Threads using sem_timedwait() to maintain polling interval use 100% CPU if we not set the memory to zero SDK-77724 
     */ 
    sal_memset(s, 0, sizeof(wrapped_sem_t));

    sem_init(&s->s, 0, initial_count);
    s->desc = desc;
    s->binary = binary;


    return (sal_sem_t) s;
}

void
sal_sem_destroy(sal_sem_t b)
{
    wrapped_sem_t *s = (wrapped_sem_t *) b;

    SAL_ASSERT(s);

    sem_destroy(&s->s);

    free(s);

}

int
sal_sem_take(sal_sem_t b, int usec)
{
    wrapped_sem_t		*s = (wrapped_sem_t *) b;
    int			err = 0;
#ifdef USE_POSIX_SEM_TIMEDWAIT
    struct timespec	ts;
#endif

    if (usec == sal_sem_FOREVER) {
        do {
            err = sem_wait(&s->s);
        } while (err != 0 && errno == EINTR);
    }
#ifdef USE_POSIX_SEM_TIMEDWAIT
    else if (_sal_compute_timeout(&ts, usec)) {
        while (1) {
            if (!sem_timedwait(&s->s, &ts)) {
                err = 0;
                break;
            }
            if (errno != EAGAIN && errno != EINTR) {
                err = errno;
                break;
            }
        }
    }
#else
    else {
	int		time_wait = 1;

	/* Retry algorithm with exponential backoff */

	for (;;) {
	    if (sem_trywait(&s->s) == 0) {
		err = 0;
		break;
	    }

	    if (errno != EAGAIN && errno != EINTR) {
		err = errno;
		break;
	    }

	    if (time_wait > usec) {
		time_wait = usec;
	    }

	    sal_usleep(time_wait);

	    usec -= time_wait;

	    if (usec == 0) {
		err = ETIMEDOUT;
		break;
	    }

	    if ((time_wait *= 2) > 100000) {
		time_wait = 100000;
	    }
	}
    }
#endif

    return err ? -1 : 0;
}

int
sal_sem_give(sal_sem_t b)
{
    wrapped_sem_t       *s = (wrapped_sem_t *) b;
    int                 err = 0;
    int                 sem_val = 0;

    /* Binary sem only post if sem_val == 0 */
    if (s->binary) {
        /* Post sem on getvalue failure */
        sem_getvalue(&s->s, &sem_val);
        if (sem_val == 0) {
            err = sem_post(&s->s);
        }
    } else {
        err = sem_post(&s->s);
    }

    return err ? -1 : 0;
}

#endif /* LINUX_SAL_SEM_OVERRIDE */

