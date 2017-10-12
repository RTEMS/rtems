/**
 * @file
 *
 * @brief Private Support Information for POSIX Threads
 * @ingroup POSIX_PTHREADS Private Threads
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/pthreadattrimpl.h>

/*
 *  The default pthreads attributes structure.
 *
 *  NOTE: Be careful .. if the default attribute set changes,
 *        _POSIX_Threads_Initialize_user_threads will need to be examined.
 */
const pthread_attr_t _POSIX_Threads_Default_attributes = {
  .is_initialized  = true,                       /* is_initialized */
  .stackaddr       = NULL,                       /* stackaddr */
  .stacksize       = 0,                          /* stacksize -- will be adjusted to minimum */
  .contentionscope = PTHREAD_SCOPE_PROCESS,      /* contentionscope */
  .inheritsched    = PTHREAD_INHERIT_SCHED,      /* inheritsched */
  .schedpolicy     = SCHED_FIFO,                 /* schedpolicy */
  .schedparam      =
  {                           /* schedparam */
    2,                        /* sched_priority */
    #if defined(_POSIX_SPORADIC_SERVER) || \
        defined(_POSIX_THREAD_SPORADIC_SERVER)
      0,                        /* sched_ss_low_priority */
      { 0L, 0 },                /* sched_ss_repl_period */
      { 0L, 0 },                /* sched_ss_init_budget */
      0                         /* sched_ss_max_repl */
    #endif
  },

  #if HAVE_DECL_PTHREAD_ATTR_SETGUARDSIZE
    .guardsize = 0,                            /* guardsize */
  #endif
  #if defined(_POSIX_THREAD_CPUTIME)
    .cputime_clock_allowed = 1,                        /* cputime_clock_allowed */
  #endif
  .detachstate             = PTHREAD_CREATE_JOINABLE,    /* detachstate */
  .affinitysetsize         =
    sizeof( _POSIX_Threads_Default_attributes.affinitysetpreallocated ),
  .affinityset             = RTEMS_DECONST(
    cpu_set_t *,
    &_POSIX_Threads_Default_attributes.affinitysetpreallocated
  ),
  .affinitysetpreallocated = { { -1L } }
};
