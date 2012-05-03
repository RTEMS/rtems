/*
 *  Semaphore Manager
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the Semaphore Manager.
 *  This manager utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  Directives provided are:
 *
 *     + create a semaphore
 *     + get an ID of a semaphore
 *     + delete a semaphore
 *     + acquire a semaphore
 *     + release a semaphore
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/sem.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/coresem.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/score/sysstate.h>

#include <rtems/score/interr.h>

/*
 *  rtems_semaphore_obtain
 *
 *  This directive allows a thread to acquire a semaphore.
 *
 *  Input parameters:
 *    id         - semaphore id
 *    option_set - wait option
 *    timeout    - number of ticks to wait (0 means wait forever)
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_semaphore_obtain(
  rtems_id        id,
  rtems_option    option_set,
  rtems_interval  timeout
)
{
  register Semaphore_Control     *the_semaphore;
  Objects_Locations               location;
  ISR_Level                       level;

  the_semaphore = _Semaphore_Get_interrupt_disable( id, &location, &level );
  switch ( location ) {

    case OBJECTS_LOCAL:
      if ( !_Attributes_Is_counting_semaphore(the_semaphore->attribute_set) ) {
        _CORE_mutex_Seize(
          &the_semaphore->Core_control.mutex,
          id,
          ((_Options_Is_no_wait( option_set )) ? false : true),
          timeout,
          level
        );
        return _Semaphore_Translate_core_mutex_return_code(
                  _Thread_Executing->Wait.return_code );
      }

      /* must be a counting semaphore */
      _CORE_semaphore_Seize_isr_disable(
        &the_semaphore->Core_control.semaphore,
        id,
        ((_Options_Is_no_wait( option_set )) ? false : true),
        timeout,
        &level
      );
      return _Semaphore_Translate_core_semaphore_return_code(
                  _Thread_Executing->Wait.return_code );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      return _Semaphore_MP_Send_request_packet(
          SEMAPHORE_MP_OBTAIN_REQUEST,
          id,
          option_set,
          timeout
      );
#endif

    case OBJECTS_ERROR:
      break;

  }

  return RTEMS_INVALID_ID;
}
