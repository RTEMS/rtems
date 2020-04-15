/**
 *  @file
 *
 *  @brief POSIX Delete Message Queue
 *  @ingroup POSIX_MQUEUE
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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

void _POSIX_Message_queue_Delete(
  POSIX_Message_queue_Control *the_mq,
  Thread_queue_Context        *queue_context
)
{
  if ( !the_mq->linked && the_mq->open_count == 0 ) {
    _CORE_message_queue_Close( &the_mq->Message_queue, queue_context );
    _POSIX_Message_queue_Free( the_mq );
  } else {
    _CORE_message_queue_Release( &the_mq->Message_queue, queue_context );
  }
}
