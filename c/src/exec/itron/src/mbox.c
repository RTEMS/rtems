/*
 *  ITRON 3.0 Mailbox Manager
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <itron.h>

#include <rtems/itron/mbox.h>
#include <rtems/itron/task.h>

/*
 *  _ITRON_Mailbox_Translate_core_message_queue_return_code
 *
 *  This routine translates a core message queue object status
 *  into the appropriate ITRON status code.
 */

ER _ITRON_Mailbox_Translate_core_message_queue_return_code(
  CORE_message_queue_Status status
)
{
  switch (status) {
    case CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL:
      return E_OK;
    case CORE_MESSAGE_QUEUE_STATUS_TOO_MANY:
      return E_TMOUT;
    case CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE:
      return E_PAR;
    case CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT:
      return E_TMOUT;
    case CORE_MESSAGE_QUEUE_STATUS_TIMEOUT:
      return E_TMOUT;
    default:
      return E_ID;
  }
}

/*    
 *  _ITRON_Mailbox_Manager_initialization
 *  
 *  This routine initializes all mailboxes manager related data structures.
 *
 *  Input parameters:
 *    maximum_mailboxes - maximum configured mailboxes
 *
 *  Output parameters:  NONE
 */

void _ITRON_Mailbox_Manager_initialization(
  unsigned32 maximum_mailboxes
) 
{
  _Objects_Initialize_information(
    &_ITRON_Mailbox_Information,     /* object information table */
    OBJECTS_ITRON_MAILBOXES,         /* object class */
    FALSE,                           /* TRUE if this is a global */
                                     /*   object class */
    maximum_mailboxes,               /* maximum objects of this class */
    sizeof( ITRON_Mailbox_Control ), /* size of this object's control block */
    FALSE,                           /* TRUE if names for this object */
                                     /*   are strings */
    RTEMS_MAXIMUM_NAME_LENGTH,       /* maximum length of each object's */
                                     /*   name */
    FALSE                            /* TRUE if this class is threads */
  );
    
  /*
   *  Register the MP Process Packet routine.
   *
   *  NOTE: No MP Support YET in RTEMS ITRON implementation.
   */
 
} 


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
  CORE_message_queue_Status        status = E_OK;
  unsigned32                       message_priority;
  void                            *message_contents;

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
      status = _CORE_message_queue_Submit(
        &the_mailbox->message_queue,
        &message_contents,
        sizeof(T_MSG *),
        the_mailbox->Object.id,
        NULL,          /* multiprocessing not supported */
        message_priority
     );
     break;
  }

  _ITRON_return_errorno( 
     _ITRON_Mailbox_Translate_core_message_queue_return_code(status) );
}

/*
 *  rcv_msg - Receive Message from Mailbox 
 */

ER rcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid
)
{
  return trcv_msg( ppk_msg, mbxid, TMO_FEVR );
}

/*
 *  prcv_msg - Poll and Receive Message from Mailbox
 */

ER prcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid
)
{
  return trcv_msg( ppk_msg, mbxid, TMO_POL );
}

/*
 *  trcv_msg - Receive Message from Mailbox with Timeout
 */

ER trcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid,
  TMO     tmout
)
{
  register ITRON_Mailbox_Control *the_mailbox;
  Watchdog_Interval               interval;
  boolean                         wait;
  Objects_Locations               location;
  unsigned32                      size;

  if (!ppk_msg)
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

  the_mailbox = _ITRON_Mailbox_Get( mbxid, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      return _ITRON_Mailbox_Clarify_get_id_error( mbxid );

    case OBJECTS_LOCAL:

      _CORE_message_queue_Seize(
        &the_mailbox->message_queue,
        the_mailbox->Object.id,
        ppk_msg,
        &size,
        wait,
        interval 
      );
      break;
  }

  _ITRON_return_errorno( 
    _ITRON_Mailbox_Translate_core_message_queue_return_code(
        _Thread_Executing->Wait.return_code ) );
}

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

