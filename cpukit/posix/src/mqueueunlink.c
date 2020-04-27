/**
 * @file
 *
 * @ingroup POSIX_MQUEUE Message Queues
 *
 * @brief Remove a Message Queue
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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
 *  15.2.2 Remove a Message Queue, P1003.1b-1993, p. 276
 */

int mq_unlink(
  const char *name
)
{
  POSIX_Message_queue_Control *the_mq;
  Objects_Get_by_name_error    error;
  Thread_queue_Context         queue_context;

  _Objects_Allocator_lock();

  the_mq = _POSIX_Message_queue_Get_by_name( name, NULL, &error );
  if ( the_mq == NULL ) {
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( _POSIX_Get_by_name_error( error ) );
  }

  _POSIX_Message_queue_Namespace_remove( the_mq );

  _CORE_message_queue_Acquire( &the_mq->Message_queue, &queue_context );

  the_mq->linked = false;
  _POSIX_Message_queue_Delete( the_mq, &queue_context );

  _Objects_Allocator_unlock();
  return 0;
}
