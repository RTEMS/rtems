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

/*
 *  trcv_mbf - Receive Message from MessageBuffer with Timeout
 */

ER trcv_mbf(
  VP   msg,
  INT *p_msgsz,
  ID   mbfid,
  TMO  tmout
)
{
  ITRON_Message_buffer_Control   *the_message_buffer;
  Objects_Locations               location;
  CORE_message_queue_Status       status;
  boolean                         wait;
  Watchdog_Interval               interval;

  interval = 0;
  if (tmout == TMO_POL) {
    wait = FALSE;
  } else {
    wait = TRUE;
    if (tmout != TMO_FEVR) 
      interval = TOD_MILLISECONDS_TO_TICKS(tmout);
  }

  if (wait && _ITRON_Is_in_non_task_state() ) 
    return E_CTX;

  if (!p_msgsz || !msg || tmout <= -2)
    return E_PAR;
    
  the_message_buffer = _ITRON_Message_buffer_Get(mbfid, &location);
  switch (location) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:           /* Multiprocessing not supported */
      return _ITRON_Message_buffer_Clarify_get_id_error(mbfid);

    case OBJECTS_LOCAL:
      _CORE_message_queue_Seize(
          &the_message_buffer->message_queue,
          the_message_buffer->Object.id,
          msg,
          p_msgsz,
          wait,
          interval
      );
      _Thread_Enable_dispatch();
      status = (CORE_message_queue_Status)_Thread_Executing->Wait.return_code; 
      return
        _ITRON_Message_buffer_Translate_core_message_buffer_return_code(status);
    }

    /*
     *  If multiprocessing were supported, this is where we would announce
     *  the existence of the semaphore to the rest of the system.
     */

#if defined(RTEMS_MULTIPROCESSING)
#endif
    return E_OK;
}
