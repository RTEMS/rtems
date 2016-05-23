/**
 *  @file
 *
 *  @brief RTEMS Urgent Message Queue
 *  @ingroup ClassicMessageQueue
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/messageimpl.h>

rtems_status_code rtems_message_queue_urgent(
  rtems_id    id,
  const void *buffer,
  size_t      size
)
{
  Message_queue_Control     *the_message_queue;
  ISR_lock_Context           lock_context;
  CORE_message_queue_Status  status;

  if ( buffer == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  the_message_queue = _Message_queue_Get( id, &lock_context );

  if ( the_message_queue == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Message_queue_MP_Urgent( id, buffer, size );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  _CORE_message_queue_Acquire_critical(
    &the_message_queue->message_queue,
    &lock_context
  );
  status = _CORE_message_queue_Urgent(
    &the_message_queue->message_queue,
    buffer,
    size,
    _Message_queue_Core_message_queue_mp_support,
    false,   /* sender does not block */
    0,       /* no timeout */
    &lock_context
  );

  /*
   *  Since this API does not allow for blocking sends, we can directly
   *  return the returned status.
   */

  return _Message_queue_Translate_core_message_queue_return_code( status );
}
