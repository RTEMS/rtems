/* unistd.h
 *
 *  $Id$
 */

#ifndef __POSIX_UNISTD_h
#define __POSIX_UNISTD_h

#include <rtems/posix/features.h>

/*
 *  4.8.1 Get Configurable System Variables, P1003.1b-1993, p. 96
 *
 *  NOTE: Table 4-2, Configurable System Variables, p. 96
 */

#define _SC_AIO_LISTIO_MAX          0
#define _SC_AIO_MAX                 1
#define _SC_AIO_PRIO_DELTA_MAX      2
#define _SC_ARG_MAX                 3
#define _SC_CHILD_MAX               4
#define _SC_CLK_TCK                 5
#define _SC_DELAYTIMER_MAX          6
#define _SC_MQ_OPEN_MAX             7
#define _SC_MQ_PRIO_MAX             8
#define _SC_NGROUPS_MAX             9
#define _SC_OPEN_MAX               10
#define _SC_PAGESIZE               11
#define _SC_RTSIG_MAX              12
#define _SC_SEM_NSEMS_MAX          13
#define _SC_SEM_VALUE_MAX          14
#define _SC_SIGQUEUE_MAX           15
#define _SC_STREAM_MAX             16
#define _SC_TIMER_MAX              17
#define _SC_TZNAME_MAX             18

#define _SC_ASYNCHRONOUS_IO        19
#define _SC_FSYNC                  20
#define _SC_JOB_CONTROL            21
#define _SC_MAPPED_FILES           22
#define _SC_MEMLOCK                23
#define _SC_MEMLOCK_RANGE          24
#define _SC_MEMORY_PROTECTION      25
#define _SC_MESSAGE_PASSING        26
#define _SC_PRIORITIZED_IO         27
#define _SC_REALTIME_SIGNALS       28
#define _SC_SAVED_IDS              29
#define _SC_SEMAPHORES             30
#define _SC_SHARED_MEMORY_OBJECTS  31
#define _SC_SYNCHRONIZED_IO        32
#define _SC_TIMERS                 33
#define _SC_VERSION                34

/*
 *  P1003.1c/D10, p. 52 adds the following.
 */

#define _SC_GETGR_R_SIZE_MAX             35
#define _SC_GETPW_R_SIZE_MAX             36
#define _SC_LOGIN_NAME_MAX               37
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 38
#define _SC_THREAD_KEYS_MAX              39
#define _SC_THREAD_STACK_MIN             40
#define _SC_THREAD_THREADS_MAX           41
#define _SC_TTY_NAME_MAX                 42

#define _SC_THREADS                      43
#define _SC_THREAD_ATTR_STACKADDR        44
#define _SC_THREAD_ATTR_STACKSIZE        45
#define _SC_THREAD_PRIORITY_SCHEDULING   46
#define _SC_THREAD_PRIO_INHERIT          47
#define _SC_THREAD_PRIO_CEILING          48
#define _SC_THREAD_PROCESS_SHARED        49
#define _SC_THREAD_SAFE_FUNCTIONS        50

/* JRS: 04/02/98: _SC_THREAD_PRIO_CEILING seems to have changed names
 *                in the final standard to _SC_THREAD_PRIO_PROTECT.
 */

/*
 *  4.8.1 Get Configurable System Variables, P1003.1b-1993, p. 95
 */

long sysconf(
  int name
);

#endif
/* end of include */
