/*
 *  This test file is used to verify that the header files associated with
 *  the callout are correct.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <sched.h>

#ifndef _POSIX_PRIORITY_SCHEDULING
#error "rtems is supposed to have sched_setparam"
#endif
 
void test( void )
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
  param.ss_low_priority = 0;
  param.ss_replenish_period.tv_sec = 0;
  param.ss_replenish_period.tv_nsec = 0;
  param.ss_initial_budget.tv_sec = 0;
  param.ss_initial_budget.tv_nsec = 0;
#endif

  result = sched_setparam( pid, &param );
}
