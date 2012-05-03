/** 
 *  @file  rtems/score/schedulersimple.inl
 *
 *  This inline file contains all of the inlined routines associated with
 *  the manipulation of the priority-based scheduling structures.
 */

/*
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERSIMPLE_H
# error "Never use <rtems/score/schedulersimple.inl> directly; include <rtems/score/schedulersimple.h> instead."
#endif

#ifndef _RTEMS_SCORE_SCHEDULERSIMPLE_INL
#define _RTEMS_SCORE_SCHEDULERSIMPLE_INL

#include <rtems/score/thread.h>

/**
 *  @addtogroup ScoreScheduler
 * @{
 */

/**
 *  This routine puts @a the_thread on to the ready queue.
 *
 *  @param[in] the_ready_queue is a pointer to the ready queue head
 *  @param[in] the_thread is the thread to be blocked
 */
RTEMS_INLINE_ROUTINE void _Scheduler_simple_Ready_queue_requeue(
  Scheduler_Control *the_ready_queue,
  Thread_Control    *the_thread
)
{
  /* extract */
  _Chain_Extract_unprotected( &the_thread->Object.Node );

  /* enqueue */
  _Scheduler_simple_Ready_queue_enqueue( the_thread );
}

/**@}*/

#endif
/* end of include file */
