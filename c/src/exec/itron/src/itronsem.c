/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <itron.h>

#include <rtems/itron/semaphore.h>
#include <rtems/itron/task.h>
#include <rtems/score/tod.h>

/*
 *  _ITRON_Semaphore_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine initializes all semaphore manager related data structures.
 *
 *  Input parameters:
 *    maximum_semaphores - maximum configured semaphores
 *
 *  Output parameters:  NONE
 */

void _ITRON_Semaphore_Manager_initialization(
  unsigned32 maximum_semaphores
)
{
  _Objects_Initialize_information(
    &_ITRON_Semaphore_Information,     /* object information table */
    OBJECTS_ITRON_SEMAPHORES,          /* object class */
    FALSE,                             /* TRUE if this is a global */
                                       /*   object class */
    maximum_semaphores,                /* maximum objects of this class */
    sizeof( ITRON_Semaphore_Control ), /* size of this object's control block */
    FALSE,                             /* TRUE if names for this object */
                                       /*   are strings */
    RTEMS_MAXIMUM_NAME_LENGTH,         /* maximum length of each object's */
                                       /*   name */
    FALSE                              /* TRUE if this class is threads */
  );

  /*
   *  Register the MP Process Packet routine.
   *
   *  NOTE: No MP Support YET in RTEMS ITRON implementation.
   */

}

/*
 *  cre_sem - Create Semaphore
 *
 *  This function implements the ITRON 3.0 cre_sem() service.
 */

ER cre_sem(
  ID      semid,
  T_CSEM *pk_csem
)
{
  CORE_semaphore_Attributes   the_semaphore_attributes;
  ITRON_Semaphore_Control    *the_semaphore;

  /*
   *  Bad pointer to the attributes structure
   */

  if ( !pk_csem )
    return E_PAR;

  /*
   *  Bits were set that were note defined.
   */

  if ( pk_csem->sematr & _ITRON_SEMAPHORE_UNUSED_ATTRIBUTES )
    return E_RSATR;

  /*
   *  Initial semaphore count exceeds the maximum.
   */

  if ( pk_csem->isemcnt > pk_csem->maxsem )
    return E_PAR;

  /*
   *  This error is not in the specification but this condition
   *  does not make sense.
   */

  if ( pk_csem->maxsem == 0 )
    return E_PAR;

  _Thread_Disable_dispatch();             /* prevents deletion */

  the_semaphore = _ITRON_Semaphore_Allocate( semid );
  if ( !the_semaphore ) {
    _Thread_Enable_dispatch();
    return _ITRON_Semaphore_Clarify_allocation_id_error( semid );
  }

  if ( pk_csem->sematr & TA_TPRI )
    the_semaphore_attributes.discipline = CORE_SEMAPHORE_DISCIPLINES_PRIORITY;
  else
    the_semaphore_attributes.discipline = CORE_SEMAPHORE_DISCIPLINES_FIFO;

  the_semaphore_attributes.maximum_count = pk_csem->maxsem;

  _CORE_semaphore_Initialize(
    &the_semaphore->semaphore,
    OBJECTS_ITRON_SEMAPHORES,
    &the_semaphore_attributes,
    pk_csem->isemcnt,
    NULL                           /* Multiprocessing not supported */
  );

  _ITRON_Objects_Open( &_ITRON_Semaphore_Information, &the_semaphore->Object );

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
 *  del_sem - Delete Semaphore
 *
 *  This function implements the ITRON 3.0 del_sem() service.
 */

ER del_sem(
  ID semid
)
{
  ITRON_Semaphore_Control *the_semaphore;
  Objects_Locations        location;
  
  the_semaphore = _ITRON_Semaphore_Get( semid, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:               /* Multiprocessing not supported */
    case OBJECTS_ERROR:
      return _ITRON_Semaphore_Clarify_get_id_error( semid );
 
    case OBJECTS_LOCAL:
      _CORE_semaphore_Flush(
        &the_semaphore->semaphore,
        NULL,                          /* Multiprocessing not supported */
        CORE_SEMAPHORE_WAS_DELETED
      );

      _ITRON_Objects_Close(
        &_ITRON_Semaphore_Information,
        &the_semaphore->Object
      );

      _ITRON_Semaphore_Free( the_semaphore );

  /*
   *  If multiprocessing were supported, this is where we would announce
   *  the destruction of the semaphore to the rest of the system.
   */

#if defined(RTEMS_MULTIPROCESSING)
#endif

    _Thread_Enable_dispatch();
    return E_OK;

  }
  return E_OK;
}

/*
 *  sig_sem - Signal Semaphore
 *
 *  This function implements the ITRON 3.0 sig_sem() service.
 */

ER sig_sem(
  ID semid
)
{
  ITRON_Semaphore_Control *the_semaphore;
  Objects_Locations        location;
  CORE_semaphore_Status    status;

  the_semaphore = _ITRON_Semaphore_Get( semid, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:               /* Multiprocessing not supported */
    case OBJECTS_ERROR:
      return _ITRON_Semaphore_Clarify_get_id_error( semid );

    case OBJECTS_LOCAL:
      /*
       *  XXX maxsemcnt
       */

      status = _CORE_semaphore_Surrender(
        &the_semaphore->semaphore,
        the_semaphore->Object.id,
        NULL                       /* Multiprocessing not supported */
      );
      _Thread_Enable_dispatch();
      return _ITRON_Semaphore_Translate_core_semaphore_return_code( status );
  }
  return E_OK;
}

/*
 *  wai_sem - Wait on Semaphore 
 *
 *  This function implements the ITRON 3.0 wai_sem() service.
 */

ER wai_sem(
  ID semid
)
{
  return twai_sem( semid, TMO_FEVR );
}

/*
 *  preq_sem - Poll and Request Semaphore
 *
 *  This function implements the ITRON 3.0 preq_sem() service.
 */

ER preq_sem(
  ID semid
)
{ 
  return twai_sem( semid, TMO_POL );
}

/*
 *  twai_sem - Wait on Semaphore with Timeout
 *
 *  This function implements the ITRON 3.0 twai_sem() service.
 */

ER twai_sem(
  ID semid,
  TMO tmout
)
{
  ITRON_Semaphore_Control *the_semaphore;
  Objects_Locations        location;
  Watchdog_Interval        interval;
  boolean                  wait;
  CORE_semaphore_Status    status;
  
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
  
  the_semaphore = _ITRON_Semaphore_Get( semid, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:               /* Multiprocessing not supported */
    case OBJECTS_ERROR:
      return _ITRON_Semaphore_Clarify_get_id_error( semid );

    case OBJECTS_LOCAL:
      _CORE_semaphore_Seize(
        &the_semaphore->semaphore,
        the_semaphore->Object.id,
        wait,                           /* wait for a timeout */
        interval                        /* timeout value */
      );
      _Thread_Enable_dispatch();
      status = (CORE_semaphore_Status) _Thread_Executing->Wait.return_code;
      return _ITRON_Semaphore_Translate_core_semaphore_return_code( status );
  }
  return E_OK;
}

/*
 *  ref_sem - Reference Semaphore Status
 *
 *  This function implements the ITRON 3.0 ref_sem() service.
 */

ER ref_sem(
  ID      semid,
  T_RSEM *pk_rsem
)
{
  ITRON_Semaphore_Control *the_semaphore;
  Objects_Locations        location;
  
  if ( !pk_rsem )
    return E_PAR;   /* XXX check this error code */

  the_semaphore = _ITRON_Semaphore_Get( semid, &location );
  switch ( location ) {   
    case OBJECTS_REMOTE:               /* Multiprocessing not supported */
    case OBJECTS_ERROR:
      return _ITRON_Semaphore_Clarify_get_id_error( semid );
  
    case OBJECTS_LOCAL:
      /*
       *  Fill in the current semaphore count
       */

      pk_rsem->semcnt = _CORE_semaphore_Get_count( &the_semaphore->semaphore );

      /*
       *  Fill in whether or not there is a waiting task
       */

      if ( !_Thread_queue_First( &the_semaphore->semaphore.Wait_queue ) )
        pk_rsem->wtsk = FALSE;
      else
        pk_rsem->wtsk = TRUE;

      _Thread_Enable_dispatch();
      return E_OK;
  }   
  return E_OK;
}

