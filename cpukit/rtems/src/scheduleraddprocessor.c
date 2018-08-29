/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/config.h>

rtems_status_code rtems_scheduler_add_processor(
  rtems_id scheduler_id,
  uint32_t cpu_index
)
{
  uint32_t                 scheduler_index;
#if defined(RTEMS_SMP)
  Per_CPU_Control         *cpu;
  rtems_status_code        status;
#endif

  scheduler_index = _Scheduler_Get_index_by_id( scheduler_id );

  if ( scheduler_index >= _Scheduler_Count ) {
    return RTEMS_INVALID_ID;
  }

  if ( cpu_index >= rtems_configuration_get_maximum_processors() ) {
    return RTEMS_NOT_CONFIGURED;
  }

#if defined(RTEMS_SMP)
  cpu = _Per_CPU_Get_by_index( cpu_index );

  if ( _Scheduler_Initial_assignments[ cpu_index ].scheduler == NULL ) {
    return RTEMS_NOT_CONFIGURED;
  }

  if ( !_Per_CPU_Is_processor_online( cpu ) ) {
    return RTEMS_INCORRECT_STATE;
  }

  _Objects_Allocator_lock();

  if ( cpu->Scheduler.control == NULL ) {
    const Scheduler_Control *scheduler;
    Scheduler_Context       *scheduler_context;
    Priority_Control         idle_priority;
    Thread_Control          *idle;
    Scheduler_Node          *scheduler_node;
    ISR_lock_Context         lock_context;
    Per_CPU_Control         *cpu_self;

    scheduler = &_Scheduler_Table[ scheduler_index ];
    scheduler_context = _Scheduler_Get_context( scheduler );
    idle_priority =
      _Scheduler_Map_priority( scheduler, scheduler->maximum_priority );

    idle = cpu->Scheduler.idle_if_online_and_unused;
    _Assert( idle != NULL );
    cpu->Scheduler.idle_if_online_and_unused = NULL;

    idle->Scheduler.home_scheduler = scheduler;
    idle->Start.initial_priority = idle_priority;
    scheduler_node =
      _Thread_Scheduler_get_node_by_index( idle, scheduler_index );
    _Priority_Node_set_priority( &idle->Real_priority, idle_priority );
    _Priority_Initialize_one(
      &scheduler_node->Wait.Priority,
      &idle->Real_priority
    );
    _Assert( _Chain_Is_empty( &idle->Scheduler.Wait_nodes ) );
    _Chain_Initialize_one(
      &idle->Scheduler.Wait_nodes,
      &scheduler_node->Thread.Wait_node
    );
    _Assert( _Chain_Is_empty( &idle->Scheduler.Scheduler_nodes ) );
    _Chain_Initialize_one(
      &idle->Scheduler.Scheduler_nodes,
      &scheduler_node->Thread.Scheduler_node.Chain
    );

    _ISR_lock_ISR_disable( &lock_context );
    _Scheduler_Acquire_critical( scheduler, &lock_context );
    _Processor_mask_Set( &scheduler_context->Processors, cpu_index );
    cpu->Scheduler.control = scheduler;
    cpu->Scheduler.context = scheduler_context;
    ( *scheduler->Operations.add_processor )( scheduler, idle );
    cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
    _Scheduler_Release_critical( scheduler, &lock_context );
    _ISR_lock_ISR_enable( &lock_context );
    _Thread_Dispatch_direct( cpu_self );
    status = RTEMS_SUCCESSFUL;
  } else {
    status = RTEMS_RESOURCE_IN_USE;
  }

  _Objects_Allocator_unlock();
  return status;
#else
  return RTEMS_RESOURCE_IN_USE;
#endif
}
