/**
 *  @file
 *
 *  @brief RTEMS Obtain Semaphore
 *  @ingroup ClassicSem
 */

/*
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
#include <rtems/rtems/attrimpl.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/optionsimpl.h>
#include <rtems/rtems/semimpl.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/coresemimpl.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif

#include <rtems/score/interr.h>

rtems_status_code rtems_semaphore_obtain(
  rtems_id        id,
  rtems_option    option_set,
  rtems_interval  timeout
)
{
  register Semaphore_Control     *the_semaphore;
  Objects_Locations               location;
  ISR_Level                       level;
  Thread_Control                 *executing;

  the_semaphore = _Semaphore_Get_interrupt_disable( id, &location, &level );
  switch ( location ) {

    case OBJECTS_LOCAL:
      executing = _Thread_Executing;
      if ( !_Attributes_Is_counting_semaphore(the_semaphore->attribute_set) ) {
        _CORE_mutex_Seize(
          &the_semaphore->Core_control.mutex,
          executing,
          id,
          ((_Options_Is_no_wait( option_set )) ? false : true),
          timeout,
          level
        );
        _Objects_Put_for_get_isr_disable( &the_semaphore->Object );
        return _Semaphore_Translate_core_mutex_return_code(
                  executing->Wait.return_code );
      }

      /* must be a counting semaphore */
      _CORE_semaphore_Seize_isr_disable(
        &the_semaphore->Core_control.semaphore,
        executing,
        id,
        ((_Options_Is_no_wait( option_set )) ? false : true),
        timeout,
        level
      );
      _Objects_Put_for_get_isr_disable( &the_semaphore->Object );
      return _Semaphore_Translate_core_semaphore_return_code(
                  executing->Wait.return_code );

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
