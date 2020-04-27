/**
 * @file
 *
 * @ingroup POSIX_MQUEUE
 *
 * @brief Notify Process that a Message is Available on a Queue
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

#include <signal.h>

static void _POSIX_Message_queue_Notify_handler(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Context       *queue_context
)
{
  POSIX_Message_queue_Control *the_mq;
  int                          signo;

  the_mq = RTEMS_CONTAINER_OF(
    the_message_queue,
    POSIX_Message_queue_Control,
    Message_queue
  );

  signo = the_mq->notification.sigev_signo;
  _CORE_message_queue_Set_notify( &the_mq->Message_queue, NULL );
  _CORE_message_queue_Release( &the_mq->Message_queue, queue_context );

  kill( getpid(), signo );
}

int mq_notify(
  mqd_t                  mqdes,
  const struct sigevent *notification
)
{
  POSIX_Message_queue_Control *the_mq;
  Thread_queue_Context         queue_context;

  the_mq = _POSIX_Message_queue_Get( mqdes, &queue_context );

  if ( the_mq == NULL ) {
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  _CORE_message_queue_Acquire_critical(
    &the_mq->Message_queue,
    &queue_context
  );

  if ( the_mq->open_count == 0 ) {
    _CORE_message_queue_Release( &the_mq->Message_queue, &queue_context );
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  if ( notification != NULL ) {
    if ( _CORE_message_queue_Is_notify_enabled( &the_mq->Message_queue ) ) {
      _CORE_message_queue_Release( &the_mq->Message_queue, &queue_context );
      rtems_set_errno_and_return_minus_one( EBUSY );
    }

    the_mq->notification = *notification;

    _CORE_message_queue_Set_notify(
      &the_mq->Message_queue,
      _POSIX_Message_queue_Notify_handler
    );
  } else {
    _CORE_message_queue_Set_notify( &the_mq->Message_queue, NULL );
  }

  _CORE_message_queue_Release( &the_mq->Message_queue, &queue_context );
  return 0;
}
