/*
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

#include <rtems/itron/semaphore.h>
#include <rtems/itron/task.h>
#include <rtems/score/tod.h>

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

