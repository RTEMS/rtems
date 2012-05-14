/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>

#ifndef _POSIX_THREAD_PRIORITY_SCHEDULING
#error "RTEMS is supposed to have pthread_setschedparam"
#endif

int test( void );

int test( void )
{
  pthread_t           thread = 0;
  int                 policy;
  struct sched_param  param;
  int                 result;

  policy = SCHED_OTHER;
  policy = SCHED_FIFO;
  policy = SCHED_RR;
#ifdef _POSIX_SPORADIC_SERVER
  policy = SCHED_SPORADIC;
#endif

  /*
   *  really should use sched_get_priority_min() and sched_get_priority_max()
   */

  param.sched_priority = 0;
#ifdef _POSIX_SPORADIC_SERVER
  param.sched_ss_low_priority = 0;
  param.sched_ss_repl_period.tv_sec = 0;
  param.sched_ss_repl_period.tv_nsec = 0;
  param.sched_ss_init_budget.tv_sec = 0;
  param.sched_ss_init_budget.tv_nsec = 0;
#endif

  result = pthread_setschedparam( thread, policy, &param );

  return result;
}
