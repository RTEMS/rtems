/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Function closes the Message Queue 
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

/*
 *
 *  15.2.2 Close a Message Queue, P1003.1b-1993, p. 275
 */

int mq_close(
  mqd_t  mqdes
)
{
  POSIX_Message_queue_Control *the_mq;
  Thread_queue_Context         queue_context;

  _Objects_Allocator_lock();
  the_mq = _POSIX_Message_queue_Get( mqdes, &queue_context );

  if ( the_mq == NULL ) {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  _CORE_message_queue_Acquire_critical(
    &the_mq->Message_queue,
    &queue_context
  );

  if ( the_mq->open_count == 0 ) {
    _CORE_message_queue_Release( &the_mq->Message_queue, &queue_context );
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( EBADF );
  }

  the_mq->open_count -= 1;
  _POSIX_Message_queue_Delete( the_mq, &queue_context );

  _Objects_Allocator_unlock();
  return 0;
}
