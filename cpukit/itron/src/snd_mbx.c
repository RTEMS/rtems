/*
 *  ITRON 3.0 Mailbox Manager
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

#include <rtems/itron/mbox.h>
#include <rtems/itron/task.h>

/*
 *  snd_msg - Send Message to Mailbox
 */

ER snd_msg(
  ID     mbxid,
  T_MSG *pk_msg
)
{
  register ITRON_Mailbox_Control *the_mailbox;
  Objects_Locations                location;
  unsigned32                       message_priority;
  void                            *message_contents;
  CORE_message_queue_Status        msg_status;

  if ( !pk_msg )
    return E_PAR;

  the_mailbox = _ITRON_Mailbox_Get( mbxid, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      return _ITRON_Mailbox_Clarify_get_id_error( mbxid );

    case OBJECTS_LOCAL:
      if ( the_mailbox->do_message_priority )
        message_priority = pk_msg->msgpri;
      else
        message_priority = CORE_MESSAGE_QUEUE_SEND_REQUEST;

      message_contents = pk_msg;
      msg_status = _CORE_message_queue_Submit(
        &the_mailbox->message_queue,
        &message_contents,
        sizeof(T_MSG *),
        the_mailbox->Object.id,
        NULL,          /* multiprocessing not supported */
        message_priority,
        FALSE,     /* do not allow sender to block */
        0          /* no timeout */
     );
     break;
  }

  _ITRON_return_errorno( 
     _ITRON_Mailbox_Translate_core_message_queue_return_code( msg_status )
  );
}
