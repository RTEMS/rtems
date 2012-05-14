/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-2011.
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
#include <rtems/config.h>
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/config.h>

#if defined(RTEMS_SMP)
  #include <rtems/bspsmp.h>
#endif

/*
 *  _Thread_Handler_initialization
 *
 *  This routine initializes all thread manager related data structures.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _Thread_Handler_initialization(void)
{
  uint32_t ticks_per_timeslice =
    rtems_configuration_get_ticks_per_timeslice();
  uint32_t maximum_extensions =
    rtems_configuration_get_maximum_extensions();
  rtems_stack_allocate_init_hook stack_allocate_init_hook =
    rtems_configuration_get_stack_allocate_init_hook();
  uint32_t     maximum_internal_threads;
  #if defined(RTEMS_MULTIPROCESSING)
    uint32_t maximum_proxies =
      _Configuration_MP_table->maximum_proxies;
  #endif

  if ( rtems_configuration_get_stack_allocate_hook() == NULL ||
       rtems_configuration_get_stack_free_hook() == NULL)
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      true,
      INTERNAL_ERROR_BAD_STACK_HOOK
    );

  if ( stack_allocate_init_hook != NULL )
    (*stack_allocate_init_hook)( rtems_configuration_get_stack_space_size() );

  _Thread_Dispatch_necessary = false;
  _Thread_Executing         = NULL;
  _Thread_Heir              = NULL;
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  _Thread_Allocated_fp      = NULL;
#endif

  _Thread_Maximum_extensions = maximum_extensions;

  _Thread_Ticks_per_timeslice  = ticks_per_timeslice;

  #if defined(RTEMS_MULTIPROCESSING)
    _Thread_MP_Handler_initialization( maximum_proxies );
  #endif

  /*
   *  Initialize the internal class of threads.  We need an IDLE thread
   *  per CPU in an SMP system.  In addition, if this is a loosely
   *  coupled multiprocessing system, account for the MPCI Server Thread.
   */
  #if defined(RTEMS_SMP)
    maximum_internal_threads = rtems_configuration_smp_maximum_processors;
  #else
    maximum_internal_threads = 1;
  #endif

  #if defined(RTEMS_MULTIPROCESSING)
    if ( _System_state_Is_multiprocessing )
      maximum_internal_threads += 1;
  #endif

  _Objects_Initialize_information(
    &_Thread_Internal_information,
    OBJECTS_INTERNAL_API,
    OBJECTS_INTERNAL_THREADS,
    maximum_internal_threads,
    sizeof( Thread_Control ),
                                /* size of this object's control block */
    false,                      /* true if names for this object are strings */
    8                           /* maximum length of each object's name */
    #if defined(RTEMS_MULTIPROCESSING)
      ,
      false,                      /* true if this is a global object class */
      NULL                        /* Proxy extraction support callout */
    #endif
  );

}
