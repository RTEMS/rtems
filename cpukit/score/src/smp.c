/**
 *  @file
 *
 *  @brief SMP Support
 *  @ingroup Score
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

#include <rtems/score/smpimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>
#include <rtems/config.h>

static void _SMP_Check_scheduler_configuration( void )
{
  size_t n = _Scheduler_Count;
  size_t i;

  for ( i = 0 ; i < n ; ++i ) {
    const Scheduler_Control *scheduler = &_Scheduler_Table[ i ];

    if ( scheduler->context->processor_count == 0 ) {
      _SMP_Fatal( SMP_FATAL_SCHEDULER_WITHOUT_PROCESSORS );
    }
  }
}

static void _SMP_Start_processors( uint32_t cpu_count )
{
  uint32_t cpu_index_self = _SMP_Get_current_processor();
  uint32_t cpu_index;


  for ( cpu_index = 0 ; cpu_index < cpu_count; ++cpu_index ) {
    const Scheduler_Assignment *assignment =
      _Scheduler_Get_assignment( cpu_index );
    Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );
    bool started;

    if ( cpu_index != cpu_index_self ) {
      if ( _Scheduler_Should_start_processor( assignment ) ) {
        started = _CPU_SMP_Start_processor( cpu_index );

        if ( !started && _Scheduler_Is_mandatory_processor( assignment ) ) {
          _SMP_Fatal( SMP_FATAL_START_OF_MANDATORY_PROCESSOR_FAILED );
        }
      } else {
        started = false;
      }
    } else {
      started = true;

      if ( !_Scheduler_Should_start_processor( assignment ) ) {
        _SMP_Fatal( SMP_FATAL_BOOT_PROCESSOR_NOT_ASSIGNED_TO_SCHEDULER );
      }
    }

    cpu->started = started;

    if ( started ) {
      ++assignment->scheduler->context->processor_count;
    }
  }

  _SMP_Check_scheduler_configuration();
}

void _SMP_Handler_initialize( void )
{
  uint32_t cpu_max = rtems_configuration_get_maximum_processors();
  uint32_t cpu_count;
  uint32_t cpu_index;

  for ( cpu_index = 0 ; cpu_index < cpu_max; ++cpu_index ) {
    Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

    _SMP_ticket_lock_Initialize( &cpu->Lock, "per-CPU" );
  }

  /*
   * Discover and initialize the secondary cores in an SMP system.
   */

  cpu_count = _CPU_SMP_Initialize();
  cpu_count = cpu_count < cpu_max ? cpu_count : cpu_max;
  _SMP_Processor_count = cpu_count;

  for ( cpu_index = cpu_count ; cpu_index < cpu_max; ++cpu_index ) {
    const Scheduler_Assignment *assignment =
      _Scheduler_Get_assignment( cpu_index );

    if ( _Scheduler_Is_mandatory_processor( assignment ) ) {
      _SMP_Fatal( SMP_FATAL_MANDATORY_PROCESSOR_NOT_PRESENT );
    }
  }

  _SMP_Start_processors( cpu_count );

  _CPU_SMP_Finalize_initialization( cpu_count );
}

void _SMP_Request_start_multitasking( void )
{
  Per_CPU_Control *self_cpu = _Per_CPU_Get();
  uint32_t cpu_count = _SMP_Get_processor_count();
  uint32_t cpu_index;

  _Per_CPU_State_change( self_cpu, PER_CPU_STATE_READY_TO_START_MULTITASKING );

  for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
    Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

    _Per_CPU_State_change( cpu, PER_CPU_STATE_REQUEST_START_MULTITASKING );
  }
}

void _SMP_Start_multitasking_on_secondary_processor( void )
{
  Per_CPU_Control *self_cpu = _Per_CPU_Get();

  if ( !_Per_CPU_Is_processor_started( self_cpu ) ) {
    _SMP_Fatal( SMP_FATAL_MULTITASKING_START_ON_UNASSIGNED_PROCESSOR );
  }

  _Per_CPU_State_change( self_cpu, PER_CPU_STATE_READY_TO_START_MULTITASKING );

  _Thread_Start_multitasking();
}

void _SMP_Request_shutdown( void )
{
  Per_CPU_Control *self_cpu = _Per_CPU_Get();

  _Per_CPU_State_change( self_cpu, PER_CPU_STATE_SHUTDOWN );

  /*
   * We have to drop the Giant lock here in order to give other processors the
   * opportunity to receive the inter-processor interrupts issued previously.
   * In case the executing thread still holds SMP locks, then other processors
   * already waiting for this SMP lock will spin forever.
   */
  _Giant_Drop( self_cpu );
}

void _SMP_Send_message( uint32_t cpu_index, uint32_t message )
{
  Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );
  ISR_Level level;

  _Per_CPU_ISR_disable_and_acquire( cpu, level );
  cpu->message |= message;
  _Per_CPU_Release_and_ISR_enable( cpu, level );

  _CPU_SMP_Send_interrupt( cpu_index );
}

void _SMP_Broadcast_message( uint32_t message )
{
  uint32_t cpu_count = _SMP_Get_processor_count();
  uint32_t cpu_index_self = _SMP_Get_current_processor();
  uint32_t cpu_index;

  _Assert( _Debug_Is_thread_dispatching_allowed() );

  for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
    if ( cpu_index != cpu_index_self ) {
      _SMP_Send_message( cpu_index, message );
    }
  }
}
