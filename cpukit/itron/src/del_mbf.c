/*
 *  ITRON Message Buffer Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

#include <rtems/itron/msgbuffer.h>
#include <rtems/itron/task.h>

/*
 *  del_mbf - Delete MessageBuffer
 */

ER del_mbf(
  ID mbfid
)
{
  ITRON_Message_buffer_Control  *the_message_buffer;
  Objects_Locations              location;

  the_message_buffer = _ITRON_Message_buffer_Get(mbfid, &location);
  switch (location) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:           /* Multiprocessing not supported */
      return _ITRON_Message_buffer_Clarify_get_id_error(mbfid);

    case OBJECTS_LOCAL:
      _CORE_message_queue_Flush(&the_message_buffer->message_queue);
      _ITRON_Objects_Close( &_ITRON_Message_buffer_Information,
                            &the_message_buffer->Object);
      _ITRON_Message_buffer_Free(the_message_buffer);

      /*
       *  If multiprocessing were supported, this is where we would announce
       *  the existence of the semaphore to the rest of the system.
       */

#if defined(RTEMS_MULTIPROCESSING)
#endif
      _Thread_Enable_dispatch();
      return E_OK;
  }
  
  return E_OK;
}
