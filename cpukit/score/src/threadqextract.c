/**
 * @file
 *
 * @brief Extracts Thread from Thread Queue
 *
 * @ingroup ScoreThreadQ
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

bool _Thread_queue_Extract(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  /*
   * Can not use indirect function pointer here since Extract priority
   * is a macro and the underlying methods do not have the same signature.
   */
  if  ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_PRIORITY )
    return _Thread_queue_Extract_priority( the_thread );
  else /* must be THREAD_QUEUE_DISCIPLINE_FIFO */
    return _Thread_queue_Extract_fifo( the_thread );

}
