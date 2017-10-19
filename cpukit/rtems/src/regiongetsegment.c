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

static void _Region_Enqueue_callout(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  Region_Control *the_region;

  _Thread_queue_Add_timeout_ticks(
    queue,
    the_thread,
    cpu_self,
    queue_context
  );

  the_region = REGION_OF_THREAD_QUEUE_QUEUE( queue );
  _Region_Unlock( the_region );
}

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
      Thread_queue_Context  queue_context;
      Thread_Control       *executing;

      _Thread_queue_Context_initialize( &queue_context );
      _Thread_queue_Acquire( &the_region->Wait_queue, &queue_context );

      executing  = _Thread_Executing;
      executing->Wait.count           = size;
      executing->Wait.return_argument = segment;

      /* FIXME: This is a home grown condition variable */
      _Thread_queue_Context_set_thread_state(
        &queue_context,
        STATES_WAITING_FOR_SEGMENT
      );
      _Thread_queue_Context_set_timeout_ticks( &queue_context, timeout );
      _Thread_queue_Context_set_enqueue_callout(
        &queue_context,
        _Region_Enqueue_callout
      );
      _Thread_queue_Enqueue(
        &the_region->Wait_queue.Queue,
        the_region->wait_operations,
        executing,
        &queue_context
      );
      return _Status_Get_after_wait( executing );
    }
  }

  _Region_Unlock( the_region );
  return status;
}
