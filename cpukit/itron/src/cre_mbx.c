/*
 *  ITRON 3.0 Mailbox Manager
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

#include <rtems/itron/mbox.h>
#include <rtems/itron/task.h>

/*
 *  cre_mbx - Create Mailbox
 * 
 *      Creates a Mailbox according to the following spec:
 *
 * ------Parameters-------------------------
 *  ID      mbxid   MailboxID
 *  T_CMBX *pk_cmbx Packet to Create Mailbox
 * -----------------------------------------
 *   -*pk_cmbx members*-
 *    VP                exinf   ExtendedInformation
 *    ATR               mbxatr  MailboxAttributes
 *                      (the use of the following information
 *                        is implementation dependent)
 *    INT               bufcnt  BufferMessageCount
 *                      (CPU and/or implementation-dependent information
 *                         may also be included)
 *
 * ----Return Parameters--------------------
 *  ER      ercd    ErrorCode
 * -----------------------------------------
 *
 *
 * ----C Language Interface-----------------
 *  ER ercd = cre_mbx ( ID mbxid, T_CMBX *pk_cmbx ) ;
 * -----------------------------------------
 *
 */

ER cre_mbx(
  ID      mbxid,
  T_CMBX *pk_cmbx
)
{
  register ITRON_Mailbox_Control *the_mailbox;
  CORE_message_queue_Attributes   the_mailbox_attributes;

  if ( !pk_cmbx )
    return E_PAR;

  if ((pk_cmbx->mbxatr & (TA_TPRI | TA_MPRI)) != 0 )
    return E_RSATR;

  _Thread_Disable_dispatch();              /* protects object pointer */

  the_mailbox = _ITRON_Mailbox_Allocate( mbxid );
  if ( !the_mailbox ) {
    _Thread_Enable_dispatch();
    return _ITRON_Mailbox_Clarify_allocation_id_error( mbxid );
  }

  the_mailbox->count = pk_cmbx->bufcnt;
  if (pk_cmbx->mbxatr & TA_MPRI)
    the_mailbox->do_message_priority = TRUE;
  else
    the_mailbox->do_message_priority = FALSE;

  if (pk_cmbx->mbxatr & TA_TPRI)
    the_mailbox_attributes.discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY;
  else
    the_mailbox_attributes.discipline = CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO;

  if ( !_CORE_message_queue_Initialize(
           &the_mailbox->message_queue,
           OBJECTS_ITRON_MAILBOXES,
           &the_mailbox_attributes,
           the_mailbox->count,
           sizeof(T_MSG *),
           NULL ) ) {                      /* Multiprocessing not supported */
    _ITRON_Mailbox_Free(the_mailbox);
    _ITRON_return_errorno( E_OBJ );
  }

  _ITRON_Objects_Open( &_ITRON_Mailbox_Information, &the_mailbox->Object );

  /*
   *  If multiprocessing were supported, this is where we would announce
   *  the existence of the semaphore to the rest of the system.
   */

#if defined(RTEMS_MULTIPROCESSING)
#endif

  _ITRON_return_errorno( E_OK );
}
