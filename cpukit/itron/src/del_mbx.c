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
 *  del_mbx - Delete Mailbox
 *
 *
 * ------Parameters--------------
 * ID mbxid      The Mailbox's ID
 * ------------------------------
 *
 * -----Return Parameters-------
 * ER ercd       Itron Error Code
 * -----------------------------
 *
 * -----C Language Interface----
 * ER ercd = del_mbx(ID mbxid);
 * -----------------------------
 *
 */

ER del_mbx(
  ID mbxid
)
{
  register ITRON_Mailbox_Control *the_mailbox;
  Objects_Locations               location;

  the_mailbox= _ITRON_Mailbox_Get( mbxid, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      return _ITRON_Mailbox_Clarify_get_id_error( mbxid );

    case OBJECTS_LOCAL:
      _Objects_Close( &_ITRON_Mailbox_Information, &the_mailbox->Object );

      _CORE_message_queue_Close(
        &the_mailbox->message_queue,
        NULL,                      /* Multiprocessing not supported */
        CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED
      );

      _ITRON_Mailbox_Free(the_mailbox);
      break;
  }

  _ITRON_return_errorno( E_OK );
}
