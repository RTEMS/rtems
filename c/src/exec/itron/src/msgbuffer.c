/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <itron.h>

#include <rtems/itron/msgbuffer.h>
#include <rtems/itron/task.h>

ER _ITRON_Message_buffer_Translate_core_message_buffer_return_code(
    CORE_message_queue_Status status)
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
 *  _ITRON_Message_buffer_Manager_initialization
 *  
 *  This routine initializes all message buffer manager related data
 *  structures. 
 *
 *  Input parameters:
 *    maximum_message_buffers - maximum configured message buffers
 *
 *  Output parameters:  NONE
 */

void _ITRON_Message_buffer_Manager_initialization(
    unsigned32 maximum_message_buffers
    ) 
{
    _Objects_Initialize_information(
        &_ITRON_Message_buffer_Information, /* object information table */
        OBJECTS_ITRON_MESSAGE_BUFFERS,      /* object class */
        FALSE,                              /* TRUE if this is a
                                               global object class */ 
        maximum_message_buffers,            /* maximum objects of this class */
        sizeof( ITRON_Message_buffer_Control ),  /* size of this
                                                    object's control
                                                    block */ 
        FALSE,                         /* TRUE if names for this
                                          object are strings */ 
        RTEMS_MAXIMUM_NAME_LENGTH,     /* maximum length of each
                                          object's name */ 
        FALSE                          /* TRUE if this class is threads */
        );
    
    /*
     *  Register the MP Process Packet routine.
     *
     *  NOTE: No MP Support YET in RTEMS ITRON implementation.
     */
 
} 

/*
 *  cre_mbf - Create MessageBuffer
 */

ER cre_mbf(
    ID      mbfid,
    T_CMBF *pk_cmbf
    )
{
    CORE_message_queue_Attributes   the_message_queue_attributes;
    ITRON_Message_buffer_Control    *the_message_buffer;

    /*
     *  Bad pointer to the attributes structure
     */

    if ( !pk_cmbf )
        return E_PAR;

    /*
     *  Bits were set that were note defined.
     */

    if (pk_cmbf->mbfatr & ~(TA_TPRI))
        return E_RSATR;

    if (pk_cmbf->bufsz < 0 || pk_cmbf->maxmsz < 0)
        return E_PAR;
    
    if (pk_cmbf->bufsz < pk_cmbf->maxmsz)
        return E_PAR;

    _Thread_Disable_dispatch();             /* prevents deletion */

    the_message_buffer = _ITRON_Message_buffer_Allocate(mbfid);
    if ( !the_message_buffer ) {
        _Thread_Enable_dispatch();
        return _ITRON_Message_buffer_Clarify_allocation_id_error(mbfid);
    }

    if ( pk_cmbf->mbfatr & TA_TPRI )
        the_message_queue_attributes.discipline =
            CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY;
    else
        the_message_queue_attributes.discipline =
            CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO;

    _CORE_message_queue_Initialize(
        &the_message_buffer->message_queue,
        OBJECTS_ITRON_MESSAGE_BUFFERS,
        &the_message_queue_attributes,
        pk_cmbf->bufsz / pk_cmbf->maxmsz,
        pk_cmbf->maxmsz,
        NULL                           /* Multiprocessing not supported */
        );

    _ITRON_Objects_Open( &_ITRON_Message_buffer_Information,
                         &the_message_buffer->Object );
    
    /*
     *  If multiprocessing were supported, this is where we would announce
     *  the existence of the semaphore to the rest of the system.
     */

#if defined(RTEMS_MULTIPROCESSING)
#endif

    _Thread_Enable_dispatch();

    return E_OK;
}

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
        _ITRON_Objects_Close(
            &_ITRON_Message_buffer_Information,
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

/*
 *  snd_mbf - Send Message to MessageBuffer
 */

ER snd_mbf(
    ID  mbfid,
    VP  msg,
    INT msgsz
    )
{
    return E_OK;
}

/*
 *  psnd_mbf - Poll and Send Message to MessageBuffer
 */

ER psnd_mbf(
    ID  mbfid,
    VP  msg,
    INT msgsz
    )
{
    ITRON_Message_buffer_Control  *the_message_buffer;
    Objects_Locations              location;
    CORE_message_queue_Status      status;

    if (msgsz <= 0 || !msg)
        return E_PAR;
    
    the_message_buffer = _ITRON_Message_buffer_Get(mbfid, &location);
    switch (location) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:           /* Multiprocessing not supported */
        return _ITRON_Message_buffer_Clarify_get_id_error(mbfid);

    case OBJECTS_LOCAL:
        status = _CORE_message_queue_Submit(
            &the_message_buffer->message_queue,
            msg,
            msgsz,
            the_message_buffer->Object.id,
            NULL,
            CORE_MESSAGE_QUEUE_SEND_REQUEST);
        _Thread_Enable_dispatch();
        return
            _ITRON_Message_buffer_Translate_core_message_buffer_return_code(
                status);
    }

    /*
     *  If multiprocessing were supported, this is where we would announce
     *  the existence of the semaphore to the rest of the system.
     */

#if defined(RTEMS_MULTIPROCESSING)
#endif

    return E_OK;
}

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
    return E_OK;
}

/*
 *  rcv_mbf - Receive Message from MessageBuffer
 */

ER rcv_mbf(
    VP   msg,
    INT *p_msgsz,
    ID   mbfid
    )
{
    return trcv_mbf(msg, p_msgsz, mbfid, TMO_FEVR);
}

/*
 *  prcv_mbf - Poll and Receive Message from MessageBuffer
 */

ER prcv_mbf(
    VP   msg,
    INT *p_msgsz,
    ID   mbfid
    )
{
    return trcv_mbf(msg, p_msgsz, mbfid, TMO_POL);
}

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
    ITRON_Message_buffer_Control  *the_message_buffer;
    Objects_Locations              location;
    CORE_message_queue_Status      status;
    boolean                        wait;
    Watchdog_Interval              interval;

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
            interval);
        _Thread_Enable_dispatch();
	status =
            (CORE_message_queue_Status)_Thread_Executing->Wait.return_code; 
	return
            _ITRON_Message_buffer_Translate_core_message_buffer_return_code
            (status);
        
    }

    /*
     *  If multiprocessing were supported, this is where we would announce
     *  the existence of the semaphore to the rest of the system.
     */

#if defined(RTEMS_MULTIPROCESSING)
#endif
    return E_OK;
}

/*
 *  ref_mbf - Reference MessageBuffer Status
 */

ER ref_mbf(
    T_RMBF *pk_rmbf,
    ID      mbfid
    )
{
    ITRON_Message_buffer_Control *the_message_buffer;
    Objects_Locations             location;
        
    if ( !pk_rmbf )
        return E_PAR;   /* XXX check this error code */

    the_message_buffer = _ITRON_Message_buffer_Get( mbfid, &location );
    switch ( location ) {   
    case OBJECTS_REMOTE:               /* Multiprocessing not supported */
    case OBJECTS_ERROR:
        return _ITRON_Message_buffer_Clarify_get_id_error( mbfid );
  
    case OBJECTS_LOCAL:
        /*
         *  Fill in the size of message to be sent
         */

        if(the_message_buffer->message_queue.
           number_of_pending_messages == 0) {
            pk_rmbf->msgsz = 0;
        }
        else {
            CORE_message_queue_Buffer_control *the_message;
            the_message = (CORE_message_queue_Buffer_control*)
                the_message_buffer->message_queue.
                Pending_messages.first;
            pk_rmbf->msgsz = the_message->Contents.size;
        }
        
        /*
         *  Fill in the size of free buffer
         */

        pk_rmbf->frbufsz =
            (the_message_buffer->message_queue.maximum_pending_messages- 
             the_message_buffer->message_queue.number_of_pending_messages)*
            the_message_buffer->message_queue.maximum_message_size;


        /*
         *  Fill in whether or not there is a waiting task
         */

        if ( !_Thread_queue_First(
            &the_message_buffer->message_queue.Wait_queue ) ) 
            pk_rmbf->wtsk = FALSE;
        else
            pk_rmbf->wtsk =  TRUE;

        pk_rmbf->stsk = FALSE;
        _Thread_Enable_dispatch();
        return E_OK;
    }   
    return E_OK;
}
