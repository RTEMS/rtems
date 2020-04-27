/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief POSIX Message Queue and Send Support
 */

/*
 *  NOTE:  The structure of the routines is identical to that of POSIX
 *         Message_queues to leave the option of having unnamed message
 *         queues at a future date.  They are currently not part of the
 *         POSIX standard but unnamed message_queues are.  This is also
 *         the reason for the apparently unnecessary tracking of
 *         the process_shared attribute.  [In addition to the fact that
 *         it would be trivial to add pshared to the mq_attr structure
 *         and have process private message queues.]
 *
 *         This code ignores the O_RDONLY/O_WRONLY/O_RDWR flag at open
 *         time.
 *
 *  COPYRIGHT (c) 1989-2008.
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

#include <fcntl.h>

int _POSIX_Message_queue_Send_support(
  mqd_t                         mqdes,
  const char                   *msg_ptr,
  size_t                        msg_len,
  unsigned int                  msg_prio,
  const struct timespec        *abstime,
  Thread_queue_Enqueue_callout  enqueue_callout
)
{
  POSIX_Message_queue_Control *the_mq;
  Thread_queue_Context         queue_context;
  Status_Control               status;
  Thread_Control              *executing;

  /*
   * Validate the priority.
   * XXX - Do not validate msg_prio is not less than 0.
   */

  if ( msg_prio > MQ_PRIO_MAX ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  the_mq = _POSIX_Message_queue_Get( mqdes, &queue_context );

  if ( the_mq == NULL ) {
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  if ( ( the_mq->oflag & O_ACCMODE ) == O_RDONLY ) {
    _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  _Thread_queue_Context_set_enqueue_callout( &queue_context, enqueue_callout );
  _Thread_queue_Context_set_timeout_argument( &queue_context, abstime );

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
  status = _CORE_message_queue_Submit(
    &the_mq->Message_queue,
    executing,
    msg_ptr,
    msg_len,
    _POSIX_Message_queue_Priority_to_core( msg_prio ),
    ( the_mq->oflag & O_NONBLOCK ) == 0,
    &queue_context
  );
  return _POSIX_Zero_or_minus_one_plus_errno( status );
}
