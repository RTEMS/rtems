/**
 * @file rtems/posix/threadsup.h
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

#ifndef _RTEMS_POSIX_THREADSUP_H
#define _RTEMS_POSIX_THREADSUP_H

#include <sys/signal.h>
#include <rtems/score/coresem.h>
#include <rtems/score/tqdata.h>

typedef struct {
  pthread_attr_t          Attributes;
  int                     detachstate;
  Thread_queue_Control    Join_List;
  int                     schedpolicy;
  struct sched_param      schedparam;
  int                     ss_high_priority;
  Watchdog_Control        Sporadic_timer;

  sigset_t                signals_blocked;
  sigset_t                signals_pending;

  /*
   *  POSIX Cancelability
   */
  int                     cancelability_state;
  int                     cancelability_type;
  int                     cancelation_requested;
  Chain_Control           Cancellation_Handlers;

} POSIX_API_Control;

#endif
/* end of include file */
