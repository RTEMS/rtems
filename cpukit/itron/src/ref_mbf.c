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
 *  ref_mbf - Reference Message Buffer Status
 */

ER ref_mbf(
  T_RMBF *pk_rmbf,
  ID      mbfid
)
{
  ITRON_Message_buffer_Control      *the_message_buffer;
  Objects_Locations                  location;
  CORE_message_queue_Control        *the_core_msgq;
        
  if ( !pk_rmbf )
    return E_PAR;   /* XXX check this error code */

  the_message_buffer = _ITRON_Message_buffer_Get( mbfid, &location );
  switch ( location ) {   
  case OBJECTS_REMOTE:               /* Multiprocessing not supported */
  case OBJECTS_ERROR:
    return _ITRON_Message_buffer_Clarify_get_id_error( mbfid );
  
  case OBJECTS_LOCAL:
    the_core_msgq = &the_message_buffer->message_queue;

    /*
     *  Fill in the size of message to be sent
     */

    if (the_core_msgq->number_of_pending_messages == 0) {
      pk_rmbf->msgsz = 0;
    } else {
      pk_rmbf->msgsz = ((CORE_message_queue_Buffer_control *)
        the_core_msgq->Pending_messages.first)->Contents.size;
    }
        
    /*
     *  Fill in the size of free buffer
     */

    pk_rmbf->frbufsz =
      (the_core_msgq->maximum_pending_messages - 
       the_core_msgq->number_of_pending_messages) *
       the_core_msgq->maximum_message_size;


    /*
     *  Fill in whether or not there is a waiting task
     */

    if ( !_Thread_queue_First(&the_core_msgq->Wait_queue ) ) 
       pk_rmbf->wtsk = FALSE;
    else
       pk_rmbf->wtsk =  TRUE;

    pk_rmbf->stsk = FALSE;
    _Thread_Enable_dispatch();
    return E_OK;
  }   
  return E_OK;
}
