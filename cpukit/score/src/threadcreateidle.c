/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Create_idle().
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

#include <rtems/score/threadimpl.h>

#include <rtems/score/assert.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threadidledata.h>
#include <rtems/score/tls.h>
#include <rtems/score/userextimpl.h>

#include <string.h>

static void _Thread_Create_idle_for_CPU(
  Per_CPU_Control *cpu,
  uintptr_t        storage_size
)
{
  Thread_Configuration  config;
  Thread_Control       *idle;
  Status_Control        status;

  memset( &config, 0, sizeof( config ) );
  config.scheduler = _Scheduler_Get_by_CPU( cpu );
  _Assert( config.scheduler != NULL );
  config.priority = _Scheduler_Map_priority(
    config.scheduler,
    config.scheduler->maximum_priority
  );
  config.name = _Objects_Build_name( 'I', 'D', 'L', 'E' );
  config.is_fp = CPU_IDLE_TASK_IS_FP;
  config.is_preemptible = true;
  config.stack_free = _Objects_Free_nothing;
  config.stack_size = storage_size;

  /*
   * The IDLE thread stacks may be statically allocated or there may be a
   * custom allocator provided just as with user threads.
   */
  config.stack_area = ( *_Stack_Allocator_allocate_for_idle )(
    _Per_CPU_Get_index( cpu ),
    &config.stack_size
  );

  if ( config.stack_size < storage_size ) {
    _Internal_error( INTERNAL_ERROR_IDLE_THREAD_STACK_TOO_SMALL );
  }

  /*
   *  The entire workspace is zeroed during its initialization.  Thus, all
   *  fields not explicitly assigned were explicitly zeroed by
   *  _Workspace_Initialization.
   */
  idle = _Thread_Internal_allocate();
  _Assert( idle != NULL );

  status = _Thread_Initialize( &_Thread_Information, idle, &config );
  if ( status != STATUS_SUCCESSFUL ) {
    _Internal_error( INTERNAL_ERROR_IDLE_THREAD_CREATE_FAILED );
  }

  /*
   *  WARNING!!! This is necessary to "kick" start the system and
   *             MUST be done before _Thread_Start is invoked.
   */
  cpu->heir      = idle;
  cpu->executing = idle;
#if defined(RTEMS_SMP)
  cpu->ancestor = idle;
#endif

  idle->is_idle = true;
  idle->Start.Entry.adaptor = _Thread_Entry_adaptor_idle;
  idle->Start.Entry.Kinds.Idle.entry = _Thread_Idle_body;

  _Thread_Load_environment( idle );

  idle->current_state = STATES_READY;
  _Scheduler_Start_idle( config.scheduler, idle, cpu );
  _User_extensions_Thread_start( idle );
}

void _Thread_Create_idle( void )
{
  uintptr_t storage_size;
#if defined(RTEMS_SMP)
  uint32_t  cpu_max;
  uint32_t  cpu_index;
#endif

  storage_size = _TLS_Get_allocation_size() +
    CPU_IDLE_TASK_IS_FP * CONTEXT_FP_SIZE +
    _Thread_Idle_stack_size;

#if defined(RTEMS_SMP)
  cpu_max = _SMP_Get_processor_maximum();

  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

    if ( _Per_CPU_Is_processor_online( cpu ) ) {
      _Thread_Create_idle_for_CPU( cpu, storage_size );
    }
  }
#else
  _Thread_Create_idle_for_CPU( _Per_CPU_Get(), storage_size );
#endif

  _CPU_Use_thread_local_storage(
    &_Per_CPU_Get_executing( _Per_CPU_Get() )->Registers
  );
  _System_state_Set( SYSTEM_STATE_BEFORE_MULTITASKING );
}
