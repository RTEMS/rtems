/**
 * @file
 *
 * @brief Message Queue Attributes
 * @ingroup POSIXAPI
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
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/mqueueimpl.h>

/*
 *  15.2.8 Get Message Queue Attributes, P1003.1b-1993, p. 283
 */

int mq_getattr(
  mqd_t           mqdes,
  struct mq_attr *mqstat
)
{
  POSIX_Message_queue_Control *the_mq;
  Thread_queue_Context         queue_context;

  if ( mqstat == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

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

  /*
   *  Return the old values.
   */
  mqstat->mq_flags   = the_mq->oflag;
  mqstat->mq_msgsize = the_mq->Message_queue.maximum_message_size;
  mqstat->mq_maxmsg  = the_mq->Message_queue.maximum_pending_messages;
  mqstat->mq_curmsgs = the_mq->Message_queue.number_of_pending_messages;

  _CORE_message_queue_Release( &the_mq->Message_queue, &queue_context );
  return 0;
}
