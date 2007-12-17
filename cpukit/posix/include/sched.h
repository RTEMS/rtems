/**
 * @file sched.h
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


#ifndef _SCHED_H
#define _SCHED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

#if defined(_POSIX_PRIORITY_SCHEDULING)

#include <sys/types.h>
#include <time.h>
#include <sys/sched.h>
#include <pthread.h>

/*
 *  13.3.1 Set Scheduling Parameters, P1003.1b-1993, p. 252
 *
 */

int sched_setparam(
  pid_t                     pid,
  const struct sched_param *param
);

/*
 *  13.3.2 Set Scheduling Parameters, P1003.1b-1993, p. 253
 */

int sched_getparam(
  pid_t                     pid,
  const struct sched_param *param
);

/*
 *  13.3.3 Set Scheduling Policy and Scheduling Parameters,
 *         P1003.1b-1993, p. 254
 */

int sched_setscheduler(
  pid_t                     pid,
  int                       policy,
  const struct sched_param *param
);

/*
 *  13.3.4 Get Scheduling Policy, P1003.1b-1993, p. 256
 */

int sched_getscheduler(
  pid_t                     pid
);

/*
 *  13.3.6 Get Scheduling Parameter Limits, P1003.1b-1993, p. 258
 */

int sched_get_priority_max(
  int  policy
);

int sched_get_priority_min(
  int  policy
);

int sched_rr_get_interval(
  pid_t             pid,
  struct timespec  *interval
);

#endif /* _POSIX_PRIORITY_SCHEDULING */

#if defined(_POSIX_THREADS) || defined(_POSIX_PRIORITY_SCHEDULING)

/*
 *  13.3.5 Yield Processor, P1003.1b-1993, p. 257
 */

int sched_yield( void );

#endif /* _POSIX_THREADS or _POSIX_PRIORITY_SCHEDULING */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
