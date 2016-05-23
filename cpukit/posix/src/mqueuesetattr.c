/**
 * @file
 *
 * @brief Set Message Queue Attributes
 * @ingroup POSIX_MQUEUE
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

int mq_setattr(
  mqd_t                 mqdes,
  const struct mq_attr *__restrict mqstat,
  struct mq_attr       *__restrict omqstat
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

  if ( omqstat != NULL ) {
    omqstat->mq_flags   = the_mq->oflag;
    omqstat->mq_msgsize = the_mq->Message_queue.maximum_message_size;
    omqstat->mq_maxmsg  = the_mq->Message_queue.maximum_pending_messages;
    omqstat->mq_curmsgs = the_mq->Message_queue.number_of_pending_messages;
  }

  the_mq->oflag = mqstat->mq_flags;

  _CORE_message_queue_Release( &the_mq->Message_queue, &queue_context );
  return 0;
}
