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
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadqimpl.h>

void _Thread_queue_Extract_with_return_code(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread,
  uint32_t              return_code
)
{
  /*
   * Can not use indirect function pointer here since Extract priority
   * is a macro and the underlying methods do not have the same signature.
   */
  if  ( the_thread_queue->discipline == THREAD_QUEUE_DISCIPLINE_PRIORITY )
    return _Thread_queue_Extract_priority( the_thread, return_code );
  else /* must be THREAD_QUEUE_DISCIPLINE_FIFO */
    return _Thread_queue_Extract_fifo( the_thread, return_code );

}

void _Thread_queue_Extract(
  Thread_queue_Control *the_thread_queue,
  Thread_Control       *the_thread
)
{
  _Thread_queue_Extract_with_return_code(
    the_thread_queue,
    the_thread,
    the_thread->Wait.return_code
  );
}
