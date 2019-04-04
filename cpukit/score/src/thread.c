/**
 *  @file
 *
 *  @brief Initialize Thread Handler
 *  @ingroup RTEMSScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/wkspace.h>

#define THREAD_OFFSET_ASSERT( field ) \
  RTEMS_STATIC_ASSERT( \
    offsetof( Thread_Control, field ) == offsetof( Thread_Proxy_control, field ), \
    field \
  )

THREAD_OFFSET_ASSERT( Object );
THREAD_OFFSET_ASSERT( Join_queue );
THREAD_OFFSET_ASSERT( current_state );
THREAD_OFFSET_ASSERT( Real_priority );
#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
THREAD_OFFSET_ASSERT( resource_count );
#endif
THREAD_OFFSET_ASSERT( Scheduler );
THREAD_OFFSET_ASSERT( Wait );
THREAD_OFFSET_ASSERT( Timer );
#if defined(RTEMS_MULTIPROCESSING)
THREAD_OFFSET_ASSERT( receive_packet );
#endif

void _Thread_Initialize_information( Thread_Information *information )
{
  _Objects_Initialize_information( &information->Objects );

  _Freechain_Initialize(
    &information->Thread_queue_heads.Free,
    information->Thread_queue_heads.initial,
    _Objects_Get_maximum_index( &information->Objects ),
    _Thread_queue_Heads_size
  );
}

void _Thread_Handler_initialization(void)
{
  rtems_stack_allocate_init_hook stack_allocate_init_hook =
    rtems_configuration_get_stack_allocate_init_hook();
  #if defined(RTEMS_MULTIPROCESSING)
    uint32_t maximum_proxies =
      _Configuration_MP_table->maximum_proxies;
  #endif

  if ( rtems_configuration_get_stack_allocate_hook() == NULL ||
       rtems_configuration_get_stack_free_hook() == NULL)
    _Internal_error( INTERNAL_ERROR_BAD_STACK_HOOK );

  if ( stack_allocate_init_hook != NULL )
    (*stack_allocate_init_hook)( rtems_configuration_get_stack_space_size() );

  #if defined(RTEMS_MULTIPROCESSING)
    _Thread_MP_Handler_initialization( maximum_proxies );
  #endif

  /*
   *  Initialize the internal class of threads.  We need an IDLE thread
   *  per CPU in an SMP system.  In addition, if this is a loosely
   *  coupled multiprocessing system, account for the MPCI Server Thread.
   */
  _Thread_Initialize_information( &_Thread_Information );
}
