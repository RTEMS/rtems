/**
 *  @file
 *
 *  @brief Thread Queue First FIFO
 *  @ingroup ScoreThreadQ
 */

/*
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

#include <rtems/score/threadqimpl.h>
#include <rtems/score/chainimpl.h>

Thread_Control *_Thread_queue_First_fifo(
  Thread_queue_Control *the_thread_queue
)
{
  if ( !_Chain_Is_empty( &the_thread_queue->Queues.Fifo ) )
    return (Thread_Control *) _Chain_First( &the_thread_queue->Queues.Fifo );

  return NULL;
}
