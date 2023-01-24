/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSMP
 *
 * @brief This source file contains the definition of ::_SMP_Online_processors
 *   and ::_SMP_Processor_maximum and the implementation of
 *   _SMP_Handler_initialize(),  _SMP_Process_message(),
 *   _SMP_Request_shutdown(), _SMP_Request_start_multitasking(),
 *   _SMP_Send_message(), _SMP_Should_start_processor(),
 *   _SMP_Start_multitasking_on_secondary_processor(), and
 *   _SMP_Try_to_process_message().
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/smpimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/config.h>

#if CPU_USE_DEFERRED_FP_SWITCH == TRUE
  #error "deferred FP switch not implemented for SMP"
#endif

/**
 * @brief Indicates if the system is ready to start multitasking.
 *
 * Only the boot processor is allowed to change this object.  If the object has
 * a non-zero value and no fatal error occurred, then secondary processors
 * should call _Thread_Start_multitasking() to start multiprocessing.
 */
static Atomic_Uint _SMP_Ready_to_start_multitasking;

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

  cpu_max = _SMP_Get_processor_maximum();
  cpu_self = _Per_CPU_Get();

  /*
   * Wait until all other online processors reached the
   * PER_CPU_STATE_READY_TO_START_MULTITASKING state.  The waiting is done
   * without a timeout.  If secondary processors cannot reach this state, then
   * it is expected that they indicate this failure with an
   * ::SMP_MESSAGE_SHUTDOWN message or reset the system.
   */
  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    Per_CPU_Control *cpu;

    cpu = _Per_CPU_Get_by_index( cpu_index );

    if ( cpu != cpu_self && _Per_CPU_Is_processor_online( cpu ) ) {
      while (
        _Per_CPU_Get_state( cpu ) != PER_CPU_STATE_READY_TO_START_MULTITASKING
      ) {
        _SMP_Try_to_process_message(
          cpu_self,
          _Atomic_Load_ulong( &cpu_self->message, ATOMIC_ORDER_RELAXED )
        );
      }
    }
  }

  _Atomic_Store_uint(
    &_SMP_Ready_to_start_multitasking,
    1U,
    ATOMIC_ORDER_RELEASE
  );
}

bool _SMP_Should_start_processor( uint32_t cpu_index )
{
  const Scheduler_Assignment *assignment;

  assignment = _Scheduler_Get_initial_assignment( cpu_index );
  return _Scheduler_Should_start_processor( assignment );
}

static void _SMP_Wait_for_start_multitasking( Per_CPU_Control *cpu_self )
{
  unsigned int ready;

  do {
    _SMP_Try_to_process_message(
      cpu_self,
      _Atomic_Load_ulong( &cpu_self->message, ATOMIC_ORDER_RELAXED )
    );
    ready = _Atomic_Load_uint(
      &_SMP_Ready_to_start_multitasking,
      ATOMIC_ORDER_ACQUIRE
    );
  } while ( ready == 0U );
}

void _SMP_Start_multitasking_on_secondary_processor(
  Per_CPU_Control *cpu_self
)
{
  uint32_t cpu_index_self;

  cpu_index_self = _Per_CPU_Get_index( cpu_self );

  /*
   * Call fatal error and per-CPU job handlers with thread dispatching
   * disabled.
   */
  cpu_self->thread_dispatch_disable_level = 1;

  if ( cpu_index_self >= rtems_configuration_get_maximum_processors() ) {
    _SMP_Fatal( SMP_FATAL_MULTITASKING_START_ON_INVALID_PROCESSOR );
  }

  if ( !_SMP_Should_start_processor( cpu_index_self ) ) {
    _SMP_Fatal( SMP_FATAL_MULTITASKING_START_ON_UNASSIGNED_PROCESSOR );
  }

  _Per_CPU_Set_state( cpu_self, PER_CPU_STATE_READY_TO_START_MULTITASKING );
  _SMP_Wait_for_start_multitasking( cpu_self );

  if ( !_Per_CPU_Is_processor_online( cpu_self ) ) {
    _SMP_Fatal( SMP_FATAL_MULTITASKING_START_ON_NOT_ONLINE_PROCESSOR );
  }

  _Thread_Start_multitasking();
}

void _SMP_Request_shutdown( void )
{
  ISR_Level level;
  uint32_t  cpu_max;
  uint32_t  cpu_index_self;
  uint32_t  cpu_index;

  _ISR_Local_disable( level );
  (void) level;

  cpu_max = _SMP_Processor_configured_maximum;
  cpu_index_self = _SMP_Get_current_processor();

  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    Per_CPU_Control *cpu;

    cpu = _Per_CPU_Get_by_index( cpu_index );

    if ( cpu_index == cpu_index_self ) {
      _Per_CPU_Set_state( cpu, PER_CPU_STATE_SHUTDOWN );
    } else {
      _Atomic_Fetch_or_ulong(
        &cpu->message,
        SMP_MESSAGE_SHUTDOWN,
        ATOMIC_ORDER_RELEASE
      );

      if ( _Per_CPU_Get_state( cpu ) == PER_CPU_STATE_UP ) {
        _CPU_SMP_Send_interrupt( cpu_index );
      }
    }
  }
}

void _SMP_Process_message( Per_CPU_Control *cpu_self, long unsigned message )
{
  if ( ( message & SMP_MESSAGE_SHUTDOWN ) != 0 ) {
    ISR_Level level;

    _CPU_ISR_Disable( level );
    (void) level;

    /* Check the state to prevent recursive shutdowns */
    if ( _Per_CPU_Get_state( cpu_self ) != PER_CPU_STATE_SHUTDOWN ) {
      _Per_CPU_Set_state( cpu_self, PER_CPU_STATE_SHUTDOWN );
      _SMP_Fatal( SMP_FATAL_SHUTDOWN_RESPONSE );
    }
  }

  if ( ( message & SMP_MESSAGE_PERFORM_JOBS ) != 0 ) {
    _Per_CPU_Perform_jobs( cpu_self );
  }
}

void _SMP_Try_to_process_message(
  Per_CPU_Control *cpu_self,
  unsigned long    message
)
{
  if ( message != 0 ) {
    /*
     * Fetch the current message.  Only a read-modify-write operation
     * guarantees that we get an up to date message.  This is especially
     * important if the function was called using SMP_MESSAGE_FORCE_PROCESSING.
     */
    message = _Atomic_Exchange_ulong(
      &cpu_self->message,
      0,
      ATOMIC_ORDER_ACQUIRE
    );

    _SMP_Process_message( cpu_self, message );
  }
}

void _SMP_Send_message( Per_CPU_Control *cpu, unsigned long message )
{
  (void) _Atomic_Fetch_or_ulong(
    &cpu->message, message,
    ATOMIC_ORDER_RELEASE
  );

  if ( _Per_CPU_Get_state( cpu ) == PER_CPU_STATE_UP ) {
    _CPU_SMP_Send_interrupt( _Per_CPU_Get_index( cpu ) );
  }
}
