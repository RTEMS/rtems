/**
 *  @file
 *
 *  @brief RTEMS Get Region Segment
 *  @ingroup ClassicRegion
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/regionimpl.h>
#include <rtems/rtems/optionsimpl.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/statesimpl.h>

rtems_status_code rtems_region_get_segment(
  rtems_id           id,
  uintptr_t          size,
  rtems_option       option_set,
  rtems_interval     timeout,
  void              **segment
)
{
  rtems_status_code  status;
  Region_Control    *the_region;

  if ( segment == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  *segment = NULL;

  if ( size == 0 ) {
    return RTEMS_INVALID_SIZE;
  }

  the_region = _Region_Get_and_lock( id );

  if ( the_region == NULL ) {
    return RTEMS_INVALID_ID;
  }

  if ( size > the_region->maximum_segment_size ) {
    status = RTEMS_INVALID_SIZE;
  } else {
    void *the_segment;

    the_segment = _Region_Allocate_segment( the_region, size );

    if ( the_segment != NULL ) {
      *segment = the_segment;
      status = RTEMS_SUCCESSFUL;
    } else if ( _Options_Is_no_wait( option_set ) ) {
      status = RTEMS_UNSATISFIED;
    } else {
      Per_CPU_Control *cpu_self;
      Thread_Control  *executing;

      /*
       *  Switch from using the memory allocation mutex to using a
       *  dispatching disabled critical section.  We have to do this
       *  because this thread is going to block.
       */
      /* FIXME: This is a home grown condition variable */
      cpu_self = _Thread_Dispatch_disable();
      _Region_Unlock( the_region );

      executing  = _Per_CPU_Get_executing( cpu_self );

      executing->Wait.count           = size;
      executing->Wait.return_argument = segment;

      _Thread_queue_Enqueue(
        &the_region->Wait_queue,
        the_region->wait_operations,
        executing,
        STATES_WAITING_FOR_SEGMENT,
        timeout,
        2
      );

      _Thread_Dispatch_enable( cpu_self );

      return _Status_Get_after_wait( executing );
    }
  }

  _Region_Unlock( the_region );
  return status;
}
