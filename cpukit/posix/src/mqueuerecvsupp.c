/**
 * @file
 *
 * @ingroup POSIX_MQUEUE_P Message Queues Private Support Information
 *
 * @brief POSIX Message Queue Receive Support
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/mqueueimpl.h>
#include <rtems/posix/posixapi.h>

#include <fcntl.h>

THREAD_QUEUE_OBJECT_ASSERT(
  POSIX_Message_queue_Control,
  Message_queue.Wait_queue,
  POSIX_MESSAGE_QUEUE_CONTROL
);

/*
 *  _POSIX_Message_queue_Receive_support
 *
 *  NOTE: XXX Document how size, priority, length, and the buffer go
 *        through the layers.
 */

ssize_t _POSIX_Message_queue_Receive_support(
  mqd_t                         mqdes,
  char                         *msg_ptr,
  size_t                        msg_len,
  unsigned int                 *msg_prio,
  const struct timespec        *abstime,
  Thread_queue_Enqueue_callout  enqueue_callout
)
{
  POSIX_Message_queue_Control *the_mq;
  Thread_queue_Context         queue_context;
  size_t                       length_out;
  Thread_Control              *executing;
  Status_Control               status;

  the_mq = _POSIX_Message_queue_Get( mqdes, &queue_context );

  if ( the_mq == NULL ) {
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  if ( ( the_mq->oflag & O_ACCMODE ) == O_WRONLY ) {
    _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  if ( msg_len < the_mq->Message_queue.maximum_message_size ) {
    _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );
    rtems_set_errno_and_return_minus_one( EMSGSIZE );
  }

  _Thread_queue_Context_set_enqueue_callout( &queue_context, enqueue_callout );
  _Thread_queue_Context_set_timeout_argument( &queue_context, abstime );

  /*
   *  Now if something goes wrong, we return a "length" of -1
   *  to indicate an error.
   */
  length_out = -1;

  _CORE_message_queue_Acquire_critical(
    &the_mq->Message_queue,
    &queue_context
  );

  if ( the_mq->open_count == 0 ) {
    _CORE_message_queue_Release( &the_mq->Message_queue, &queue_context );
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  /*
   *  Now perform the actual message receive
   */
  executing = _Thread_Executing;
  status = _CORE_message_queue_Seize(
    &the_mq->Message_queue,
    executing,
    msg_ptr,
    &length_out,
    ( the_mq->oflag & O_NONBLOCK ) == 0,
    &queue_context
  );

  if ( status != STATUS_SUCCESSFUL ) {
    rtems_set_errno_and_return_minus_one( _POSIX_Get_error( status ) );
  }

  if ( msg_prio != NULL ) {
    *msg_prio = _POSIX_Message_queue_Priority_from_core(
      executing->Wait.count
    );
  }

  return length_out;
}
