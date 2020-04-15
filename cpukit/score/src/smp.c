/**
 *  @file
 *
 *  @brief SMP Support
 *  @ingroup RTEMSScore
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/smpimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/memory.h>
#include <rtems/score/percpudata.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/config.h>
#include <rtems/sysinit.h>

#include <string.h>

#if CPU_USE_DEFERRED_FP_SWITCH == TRUE
  #error "deferred FP switch not implemented for SMP"
#endif

Processor_mask _SMP_Online_processors;

uint32_t _SMP_Processor_maximum;

static const Scheduler_Assignment *_Scheduler_Get_initial_assignment(
  uint32_t cpu_index
)
{
  return &_Scheduler_Initial_assignments[ cpu_index ];
}

static bool _Scheduler_Is_mandatory_processor(
  const Scheduler_Assignment *assignment
)
{
  return (assignment->attributes & SCHEDULER_ASSIGN_PROCESSOR_MANDATORY) != 0;
}

static bool _Scheduler_Should_start_processor(
  const Scheduler_Assignment *assignment
)
{
  return assignment->scheduler != NULL;
}

static void _SMP_Start_processors( uint32_t cpu_max )
{
  uint32_t cpu_index_self;
  uint32_t cpu_index;

  cpu_index_self = _SMP_Get_current_processor();

  for ( cpu_index = 0 ; cpu_index < cpu_max; ++cpu_index ) {
    const Scheduler_Assignment *assignment;
    Per_CPU_Control            *cpu;
    bool                        started;

    assignment = _Scheduler_Get_initial_assignment( cpu_index );
    cpu = _Per_CPU_Get_by_index( cpu_index );

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

      cpu->boot = true;

      if ( !_Scheduler_Should_start_processor( assignment ) ) {
        _SMP_Fatal( SMP_FATAL_BOOT_PROCESSOR_NOT_ASSIGNED_TO_SCHEDULER );
      }
    }

    cpu->online = started;

    if ( started ) {
      const Scheduler_Control *scheduler;
      Scheduler_Context       *context;

      scheduler = assignment->scheduler;
      context = _Scheduler_Get_context( scheduler );

      _Processor_mask_Set( &_SMP_Online_processors, cpu_index );
      _Processor_mask_Set( &context->Processors, cpu_index );
      cpu->Scheduler.control = scheduler;
      cpu->Scheduler.context = context;
    }
  }
}

void _SMP_Handler_initialize( void )
{
  uint32_t cpu_config_max;
  uint32_t cpu_max;
  uint32_t cpu_index;

  cpu_config_max = rtems_configuration_get_maximum_processors();

  for ( cpu_index = 0 ; cpu_index < cpu_config_max; ++cpu_index ) {
    Per_CPU_Control *cpu;

    cpu = _Per_CPU_Get_by_index( cpu_index );
    _ISR_lock_Set_name( &cpu->Lock, "Per-CPU" );
    _ISR_lock_Set_name( &cpu->Jobs.Lock, "Per-CPU Jobs" );
    _ISR_lock_Set_name( &cpu->Watchdog.Lock, "Per-CPU Watchdog" );
    _Chain_Initialize_empty( &cpu->Threads_in_need_for_help );
  }

  /*
   * Discover and initialize the secondary cores in an SMP system.
   */

  cpu_max = _CPU_SMP_Initialize();
  cpu_max = cpu_max < cpu_config_max ? cpu_max : cpu_config_max;
  _SMP_Processor_maximum = cpu_max;

  for ( cpu_index = cpu_max ; cpu_index < cpu_config_max; ++cpu_index ) {
    const Scheduler_Assignment *assignment;

    assignment = _Scheduler_Get_initial_assignment( cpu_index );

    if ( _Scheduler_Is_mandatory_processor( assignment ) ) {
      _SMP_Fatal( SMP_FATAL_MANDATORY_PROCESSOR_NOT_PRESENT );
    }
  }

  _SMP_Start_processors( cpu_max );

  _CPU_SMP_Finalize_initialization( cpu_max );
}

void _SMP_Request_start_multitasking( void )
{
  Per_CPU_Control *cpu_self;
  uint32_t         cpu_max;
  uint32_t         cpu_index;

  cpu_self = _Per_CPU_Get();
  _Per_CPU_State_change( cpu_self, PER_CPU_STATE_READY_TO_START_MULTITASKING );

  cpu_max = _SMP_Get_processor_maximum();

  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    Per_CPU_Control *cpu;

    cpu = _Per_CPU_Get_by_index( cpu_index );

    if ( _Per_CPU_Is_processor_online( cpu ) ) {
      _Per_CPU_State_change( cpu, PER_CPU_STATE_REQUEST_START_MULTITASKING );
    }
  }
}

bool _SMP_Should_start_processor( uint32_t cpu_index )
{
  const Scheduler_Assignment *assignment;

  assignment = _Scheduler_Get_initial_assignment( cpu_index );
  return _Scheduler_Should_start_processor( assignment );
}

void _SMP_Start_multitasking_on_secondary_processor(
  Per_CPU_Control *cpu_self
)
{
  uint32_t cpu_index_self;

  cpu_index_self = _Per_CPU_Get_index( cpu_self );

  if ( cpu_index_self >= rtems_configuration_get_maximum_processors() ) {
    _SMP_Fatal( SMP_FATAL_MULTITASKING_START_ON_INVALID_PROCESSOR );
  }

  if ( !_SMP_Should_start_processor( cpu_index_self ) ) {
    _SMP_Fatal( SMP_FATAL_MULTITASKING_START_ON_UNASSIGNED_PROCESSOR );
  }

  _Per_CPU_State_change( cpu_self, PER_CPU_STATE_READY_TO_START_MULTITASKING );

  _Thread_Start_multitasking();
}

void _SMP_Request_shutdown( void )
{
  ISR_Level level;

  _ISR_Local_disable( level );
  (void) level;

  _Per_CPU_State_change( _Per_CPU_Get(), PER_CPU_STATE_SHUTDOWN );
}

void _SMP_Send_message( uint32_t cpu_index, unsigned long message )
{
  Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

  _Atomic_Fetch_or_ulong( &cpu->message, message, ATOMIC_ORDER_RELEASE );

  _CPU_SMP_Send_interrupt( cpu_index );
}

void _SMP_Send_message_broadcast( unsigned long message )
{
  uint32_t cpu_max;
  uint32_t cpu_index_self;
  uint32_t cpu_index;

  _Assert( _Debug_Is_thread_dispatching_allowed() );
  cpu_max = _SMP_Get_processor_maximum();
  cpu_index_self = _SMP_Get_current_processor();

  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    if (
      cpu_index != cpu_index_self
        && _Processor_mask_Is_set( &_SMP_Online_processors, cpu_index )
    ) {
      _SMP_Send_message( cpu_index, message );
    }
  }
}

void _SMP_Send_message_multicast(
  const Processor_mask *targets,
  unsigned long         message
)
{
  uint32_t cpu_max;
  uint32_t cpu_index;

  cpu_max = _SMP_Get_processor_maximum();

  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    if ( _Processor_mask_Is_set( targets, cpu_index ) ) {
      _SMP_Send_message( cpu_index, message );
    }
  }
}

static void _Per_CPU_Data_initialize( void )
{
  uintptr_t size;

  size = RTEMS_LINKER_SET_SIZE( _Per_CPU_Data );

  if ( size > 0 ) {
    const Memory_Information *mem;
    Per_CPU_Control          *cpu;
    uint32_t                  cpu_index;
    uint32_t                  cpu_max;

    mem = _Memory_Get();
    cpu = _Per_CPU_Get_by_index( 0 );
    cpu->data = RTEMS_LINKER_SET_BEGIN( _Per_CPU_Data );

    cpu_max = rtems_configuration_get_maximum_processors();

    for ( cpu_index = 1 ; cpu_index < cpu_max ; ++cpu_index ) {
      cpu = _Per_CPU_Get_by_index( cpu_index );
      cpu->data = _Memory_Allocate( mem, size, CPU_CACHE_LINE_BYTES );

      if( cpu->data == NULL ) {
        _Internal_error( INTERNAL_ERROR_NO_MEMORY_FOR_PER_CPU_DATA );
      }

      memcpy( cpu->data, RTEMS_LINKER_SET_BEGIN( _Per_CPU_Data ), size);
    }
  }
}

RTEMS_SYSINIT_ITEM(
  _Per_CPU_Data_initialize,
  RTEMS_SYSINIT_PER_CPU_DATA,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
