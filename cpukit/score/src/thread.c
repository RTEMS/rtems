/**
 *  @file
 *
 *  @brief Initialize Thread Handler
 *  @ingroup ScoreThread
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
#include <rtems/score/wkspace.h>

#define THREAD_OFFSET_ASSERT( field ) \
  RTEMS_STATIC_ASSERT( \
    offsetof( Thread_Control, field ) == offsetof( Thread_Proxy_control, field ), \
    field \
  )

THREAD_OFFSET_ASSERT( Object );
THREAD_OFFSET_ASSERT( current_state );
THREAD_OFFSET_ASSERT( current_priority );
THREAD_OFFSET_ASSERT( real_priority );
THREAD_OFFSET_ASSERT( priority_generation );
THREAD_OFFSET_ASSERT( priority_restore_hint );
THREAD_OFFSET_ASSERT( resource_count );
THREAD_OFFSET_ASSERT( Wait );
THREAD_OFFSET_ASSERT( Timer );
#if defined(RTEMS_MULTIPROCESSING)
THREAD_OFFSET_ASSERT( receive_packet );
#endif

void _Thread_Initialize_information(
  Thread_Information  *information,
  Objects_APIs         the_api,
  uint16_t             the_class,
  uint32_t             maximum,
  bool                 is_string,
  uint32_t             maximum_name_length
#if defined(RTEMS_MULTIPROCESSING)
  ,
  bool                 supports_global
#endif
)
{
  _Objects_Initialize_information(
    &information->Objects,
    the_api,
    the_class,
    maximum,
    _Thread_Control_size,
    is_string,
    maximum_name_length
    #if defined(RTEMS_MULTIPROCESSING)
      ,
      supports_global,
      NULL
    #endif
  );

  _Freechain_Initialize(
    &information->Free_thread_queue_heads,
    _Workspace_Allocate_or_fatal_error,
    _Objects_Maximum_per_allocation( maximum ),
    sizeof( Thread_queue_Heads )
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
    _Terminate(
      INTERNAL_ERROR_CORE,
      true,
      INTERNAL_ERROR_BAD_STACK_HOOK
    );

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
  _Thread_Initialize_information(
    &_Thread_Internal_information,
    OBJECTS_INTERNAL_API,
    OBJECTS_INTERNAL_THREADS,
    _Thread_Get_maximum_internal_threads(),
    false,                      /* true if names for this object are strings */
    8                           /* maximum length of each object's name */
    #if defined(RTEMS_MULTIPROCESSING)
      ,
      false                       /* true if this is a global object class */
    #endif
  );

}
