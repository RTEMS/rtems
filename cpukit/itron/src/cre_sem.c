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
