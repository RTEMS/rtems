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
 *  ref_mbx - Reference Mailbox Status
 */

ER ref_mbx(
  T_RMBX *pk_rmbx,
  ID      mbxid
)
{
  register ITRON_Mailbox_Control *the_mailbox;
  Objects_Locations               location;
  Chain_Control                  *pending;

  if ( !pk_rmbx )
    return E_PAR;

  the_mailbox = _ITRON_Mailbox_Get( mbxid, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      return _ITRON_Mailbox_Clarify_get_id_error( mbxid );

    case OBJECTS_LOCAL:

      pending = &the_mailbox->message_queue.Pending_messages;
      if ( _Chain_Is_empty( pending ) )
        pk_rmbx->pk_msg = NULL;
      else 
        pk_rmbx->pk_msg = (T_MSG *) pending->first;

      /*
       *  Fill in whether or not there is a waiting task
       */

      if ( !_Thread_queue_First( &the_mailbox->message_queue.Wait_queue ) )
        pk_rmbx->wtsk = FALSE;
      else
        pk_rmbx->wtsk = TRUE;

      break;
  }
  _ITRON_return_errorno( E_OK );
}

