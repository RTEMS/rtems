/*
 *  Thread Queue Handler
 *
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>

/*
 *  _Thread_queue_First
 *
 *  This routines returns a pointer to the first thread on the
 *  specified threadq.
 *
 *  Input parameters:
 *    the_thread_queue - pointer to thread queue
 *
 *  Output parameters:
 *    returns - first thread or NULL
 */

Thread_Control *_Thread_queue_First(
  Thread_queue_Control *the_thread_queue
)
{
  Thread_Control * (*first_p)(Thread_queue_Control *);

  if ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_PRIORITY )
      first_p = _Thread_queue_First_priority;
  else /* must be THREAD_QUEUE_DISCIPLINE_FIFO */
      first_p = _Thread_queue_First_fifo;

  return (*first_p)( the_thread_queue );
}
