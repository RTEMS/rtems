/*
 *  ITRON Message Buffer Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

#include <rtems/itron/msgbuffer.h>
#include <rtems/itron/task.h>

#include <assert.h>  /* only for blocking */

/*
 *  tsnd_mbf - Send Message to MessageBuffer with Timeout
 */

ER tsnd_mbf(
  ID  mbfid,
  VP  msg,
  INT msgsz,
  TMO tmout
)
{
  ITRON_Message_buffer_Control  *the_message_buffer;
  Objects_Locations              location;
  Watchdog_Interval              interval;
  boolean                        wait;
  CORE_message_queue_Status      msg_status;

  if (msgsz <= 0 || !msg)
    return E_PAR;

  interval = 0;
  if ( tmout == TMO_POL ) {
    wait = FALSE;
  } else {
    wait = TRUE;
    if ( tmout != TMO_FEVR )
      interval = TOD_MILLISECONDS_TO_TICKS(tmout);
  }

  if ( wait && _ITRON_Is_in_non_task_state() )
    return E_CTX;

  the_message_buffer = _ITRON_Message_buffer_Get(mbfid, &location);
  switch (location) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:           /* Multiprocessing not supported */
      return _ITRON_Message_buffer_Clarify_get_id_error(mbfid);

    case OBJECTS_LOCAL:
      /* XXX Submit needs to take into account blocking */
      msg_status = _CORE_message_queue_Submit(
        &the_message_buffer->message_queue,
        msg,
        msgsz,
        the_message_buffer->Object.id,
        NULL,
        CORE_MESSAGE_QUEUE_SEND_REQUEST,
        wait,      /* sender may block */
        interval   /* timeout interval */
      );
      _Thread_Enable_dispatch();
      return _ITRON_Message_buffer_Translate_core_message_buffer_return_code(
          msg_status
      );
    }

    /*
     *  If multiprocessing were supported, this is where we would announce
     *  the existence of the semaphore to the rest of the system.
     */

#if defined(RTEMS_MULTIPROCESSING)
#endif

    return E_OK;
}
