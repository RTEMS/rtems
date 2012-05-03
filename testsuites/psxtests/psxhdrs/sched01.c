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

#include <sched.h>

#ifndef _POSIX_PRIORITY_SCHEDULING
#error "rtems is supposed to have sched_setparam"
#endif

int test( void );

int test( void )
{
  pid_t  pid;
  struct sched_param param;
  int    result;

  pid = 0;

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

  result = sched_setparam( pid, &param );

  return result;
}
