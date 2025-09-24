/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMPCI
 *
 * @brief This source file contains the definition of
 *   ::_System_state_Is_multiprocessing, ::_MPCI_Semaphore,
 *   ::_MPCI_Remote_blocked_threads, ::_MPCI_table, ::_MPCI_Receive_server_tcb,
 *   ::_MPCI_Packet_processors and the implementation of
 *   _MPCI_Register_packet_processor(), _MPCI_Get_packet(),
 *   _MPCI_Send_process_packet(), _MPCI_Send_request_packet(),
 *   _MPCI_Send_response_packet(), _MPCI_Receive_packet(),
 *   _MPCI_Process_response(), _MPCI_Receive_server(), _MPCI_Announce(),
 *   _MPCI_Internal_packets_Send_process_packet(),
 *   _MPCI_Internal_packets_Process_packet(),
 *   _MPCI_Internal_packets_Get_packet(), and the MPCI Handler system
 *   initialization.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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

#include <rtems/score/coresemimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/mpciimpl.h>
#include <rtems/score/objectmp.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/sysinit.h>

#include <string.h>

RTEMS_STATIC_ASSERT(
  sizeof( MPCI_Internal_packet ) <= MP_PACKET_MINIMUM_PACKET_SIZE,
  MPCI_Internal_packet
);

#define MPCI_SEMAPHORE_TQ_OPERATIONS &_Thread_queue_Operations_FIFO

bool _System_state_Is_multiprocessing;

/**
 *  This is the core semaphore which the MPCI Receive Server blocks on.
 */
CORE_semaphore_Control _MPCI_Semaphore;

Thread_queue_Control _MPCI_Remote_blocked_threads = THREAD_QUEUE_INITIALIZER(
  "MPCI Remote Blocked Threads"
);

MPCI_Control *_MPCI_table;

Thread_Control *_MPCI_Receive_server_tcb;

MPCI_Packet_processor _MPCI_Packet_processors[ MP_PACKET_CLASSES_LAST + 1 ];

static void _MPCI_Handler_early_initialization( void )
{
  /*
   *  Initialize the system state based on whether this is an MP system.
   *  In an MP configuration, internally we view single processor
   *  systems as a very restricted multiprocessor system.
   */
  if ( _MPCI_Configuration.maximum_nodes > 1 ) {
    _System_state_Is_multiprocessing = true;
  }

  _Objects_MP_Handler_early_initialization();
}

static void _MPCI_Handler_initialization( void )
{
  MPCI_Control *users_mpci_table;

  _Objects_MP_Handler_initialization();

  users_mpci_table = _MPCI_Configuration.User_mpci_table;

  if ( _System_state_Is_multiprocessing && !users_mpci_table ) {
    _Internal_error( INTERNAL_ERROR_NO_MPCI );
  }

  _MPCI_table = users_mpci_table;

  if ( !_System_state_Is_multiprocessing ) {
    return;
  }

  /*
   *  Register the MP Process Packet routine.
   */

  _MPCI_Register_packet_processor(
    MP_PACKET_MPCI_INTERNAL,
    _MPCI_Internal_packets_Process_packet
  );

  /*
   *  Create the counting semaphore used by the MPCI Receive Server.
   */

  _CORE_semaphore_Initialize(
    &_MPCI_Semaphore,
    0 /* initial_value */
  );
}

static void _MPCI_Create_server( void )
{
  Thread_Entry_information entry = {
    .adaptor = _Thread_Entry_adaptor_numeric,
    .Kinds = { .Numeric = { .entry = _MPCI_Receive_server } }
  };
  Thread_Configuration config;
  Status_Control       status;
  ISR_lock_Context     lock_context;

  if ( !_System_state_Is_multiprocessing ) {
    return;
  }

  /*
   *  Initialize the MPCI Receive Server
   */

  _MPCI_Receive_server_tcb = _Thread_Internal_allocate();
  _Assert( _MPCI_Receive_server_tcb != NULL );

  memset( &config, 0, sizeof( config ) );
  config.scheduler = &_Scheduler_Table[ 0 ];
  config.name = _Objects_Build_name( 'M', 'P', 'C', 'I' );
  config.priority = PRIORITY_MINIMUM;
  config.is_fp = CPU_ALL_TASKS_ARE_FP;
  config.stack_size = _Stack_Minimum() +
                      _MPCI_Configuration.extra_mpci_receive_server_stack +
                      CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK +
                      CPU_ALL_TASKS_ARE_FP * CONTEXT_FP_SIZE;
  config.stack_area = _MPCI_Receive_server_stack;

  status = _Thread_Initialize(
    &_Thread_Information,
    _MPCI_Receive_server_tcb,
    &config
  );
  _Assert_Unused_variable_equals( status, STATUS_SUCCESSFUL );

  _ISR_lock_ISR_disable( &lock_context );
  _Thread_Start( _MPCI_Receive_server_tcb, &entry, &lock_context );
}

static void _MPCI_Initialization( void )
{
  ( *_MPCI_table->initialization )();
}

void _MPCI_Register_packet_processor(
  MP_packet_Classes     the_class,
  MPCI_Packet_processor the_packet_processor

)
{
  _MPCI_Packet_processors[ the_class ] = the_packet_processor;
}

MP_packet_Prefix *_MPCI_Get_packet( void )
{
  MP_packet_Prefix *the_packet;

  ( *_MPCI_table->get_packet )( &the_packet );

  if ( the_packet == NULL ) {
    _Internal_error( INTERNAL_ERROR_OUT_OF_PACKETS );
  }

  /*
   *  Put in a default timeout that will be used for
   *  all packets that do not otherwise have a timeout.
   */

  the_packet->timeout = MPCI_DEFAULT_TIMEOUT;

  return the_packet;
}

void _MPCI_Return_packet( MP_packet_Prefix *the_packet )
{
  ( *_MPCI_table->return_packet )( the_packet );
}

void _MPCI_Send_process_packet(
  uint32_t          destination,
  MP_packet_Prefix *the_packet
)
{
  the_packet->source_tid = _Thread_Executing->Object.id;
  the_packet->to_convert = ( the_packet->to_convert -
                             sizeof( MP_packet_Prefix ) ) /
                           sizeof( uint32_t );

  ( *_MPCI_table->send_packet )( destination, the_packet );
}

static void _MPCI_Enqueue_callout(
  Thread_queue_Queue     *queue,
  Thread_Control         *the_thread,
  struct Per_CPU_Control *cpu_self,
  Thread_queue_Context   *queue_context
)
{
  _Thread_queue_Add_timeout_ticks( queue, the_thread, cpu_self, queue_context );
  _Thread_Dispatch_unnest( cpu_self );
}

Status_Control _MPCI_Send_request_packet(
  uint32_t          destination,
  MP_packet_Prefix *the_packet,
  States_Control    extra_state
)
{
  Per_CPU_Control     *cpu_self;
  Thread_queue_Context queue_context;
  Thread_Control      *executing;

  /*
   *  See if we need a default timeout
   */

  if ( the_packet->timeout == MPCI_DEFAULT_TIMEOUT ) {
    the_packet->timeout = _MPCI_table->default_timeout;
  }

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_set_thread_state(
    &queue_context,
    STATES_WAITING_FOR_RPC_REPLY | extra_state
  );
  _Thread_queue_Context_set_timeout_ticks(
    &queue_context,
    the_packet->timeout
  );
  _Thread_queue_Context_set_enqueue_callout(
    &queue_context,
    _MPCI_Enqueue_callout
  );

  cpu_self = _Thread_Dispatch_disable();

  executing = _Per_CPU_Get_executing( cpu_self );
  executing->Wait.remote_id = the_packet->id;

  the_packet->source_tid = executing->Object.id;
  the_packet->source_priority = _Thread_Get_priority( executing );
  the_packet->to_convert = ( the_packet->to_convert -
                             sizeof( MP_packet_Prefix ) ) /
                           sizeof( uint32_t );

  ( *_MPCI_table->send_packet )( destination, the_packet );

  _Thread_queue_Acquire( &_MPCI_Remote_blocked_threads, &queue_context );
  _Thread_queue_Enqueue(
    &_MPCI_Remote_blocked_threads.Queue,
    &_Thread_queue_Operations_FIFO,
    executing,
    &queue_context
  );
  return _Thread_Wait_get_status( executing );
}

void _MPCI_Send_response_packet(
  uint32_t          destination,
  MP_packet_Prefix *the_packet
)
{
  the_packet->source_tid = _Thread_Executing->Object.id;

  ( *_MPCI_table->send_packet )( destination, the_packet );
}

MP_packet_Prefix *_MPCI_Receive_packet( void )
{
  MP_packet_Prefix *the_packet;

  ( *_MPCI_table->receive_packet )( &the_packet );

  return the_packet;
}

Thread_Control *_MPCI_Process_response( MP_packet_Prefix *the_packet )
{
  ISR_lock_Context lock_context;
  Thread_Control  *the_thread;

  the_thread = _Thread_Get( the_packet->id, &lock_context );
  _Assert( the_thread != NULL );

  /*
   * FIXME: This is broken on SMP, see
   *        https://gitlab.rtems.org/rtems/rtos/rtems/-/issues/2703
   *
   * Should use _Thread_queue_Extract_critical() instead with a handler
   * function provided by the caller of _MPCI_Process_response().  Similar to
   * the filter function in _Thread_queue_Flush_critical().
   */
  _ISR_lock_ISR_enable( &lock_context );
  _Thread_queue_Extract( the_thread );
  the_thread->Wait.return_code = the_packet->return_code;
  return the_thread;
}

/*
 *  _MPCI_Receive_server
 *
 */

void _MPCI_Receive_server( Thread_Entry_numeric_type ignored )
{
  MP_packet_Prefix     *the_packet;
  MPCI_Packet_processor the_function;
  Thread_Control       *executing;
  Thread_queue_Context  queue_context;

  executing = _Thread_Get_executing();
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_set_enqueue_do_nothing_extra( &queue_context );

  for ( ;; ) {
    executing->receive_packet = NULL;

    _ISR_lock_ISR_disable( &queue_context.Lock_context.Lock_context );
    _CORE_semaphore_Seize(
      &_MPCI_Semaphore,
      MPCI_SEMAPHORE_TQ_OPERATIONS,
      executing,
      true,
      &queue_context
    );

    for ( ;; ) {
      executing->receive_packet = NULL;

      the_packet = _MPCI_Receive_packet();

      if ( !the_packet ) {
        break;
      }

      executing->receive_packet = the_packet;

      if ( !_Mp_packet_Is_valid_packet_class( the_packet->the_class ) ) {
        break;
      }

      the_function = _MPCI_Packet_processors[ the_packet->the_class ];

      if ( !the_function ) {
        _Internal_error( INTERNAL_ERROR_BAD_PACKET );
      }

      ( *the_function )( the_packet );
    }
  }
}

void _MPCI_Announce( void )
{
  Thread_queue_Context queue_context;

  _ISR_lock_ISR_disable( &queue_context.Lock_context.Lock_context );
  (void) _CORE_semaphore_Surrender(
    &_MPCI_Semaphore,
    MPCI_SEMAPHORE_TQ_OPERATIONS,
    UINT32_MAX,
    &queue_context
  );
}

void _MPCI_Internal_packets_Send_process_packet(
  MPCI_Internal_Remote_operations operation
)
{
  MPCI_Internal_packet *the_packet;

  switch ( operation ) {
    case MPCI_PACKETS_SYSTEM_VERIFY:

      the_packet = _MPCI_Internal_packets_Get_packet();
      the_packet->Prefix.the_class = MP_PACKET_MPCI_INTERNAL;
      the_packet->Prefix.length = sizeof( MPCI_Internal_packet );
      the_packet->Prefix.to_convert = sizeof( MPCI_Internal_packet );
      the_packet->operation = operation;

      the_packet->maximum_nodes = _Objects_Maximum_nodes;

      the_packet->maximum_global_objects = _Objects_MP_Maximum_global_objects;

      _MPCI_Send_process_packet( MPCI_ALL_NODES, &the_packet->Prefix );
      break;
  }
}

/*
 *  _MPCI_Internal_packets_Send_request_packet
 *
 *  This subprogram is not needed since there are no request
 *  packets to be sent by this manager.
 *
 */

/*
 *  _MPCI_Internal_packets_Send_response_packet
 *
 *  This subprogram is not needed since there are no response
 *  packets to be sent by this manager.
 *
 */

void _MPCI_Internal_packets_Process_packet(
  MP_packet_Prefix *the_packet_prefix
)
{
  MPCI_Internal_packet *the_packet;
  uint32_t              maximum_nodes;
  uint32_t              maximum_global_objects;

  the_packet = (MPCI_Internal_packet *) the_packet_prefix;

  switch ( the_packet->operation ) {
    case MPCI_PACKETS_SYSTEM_VERIFY:

      maximum_nodes = the_packet->maximum_nodes;
      maximum_global_objects = the_packet->maximum_global_objects;
      if (
        maximum_nodes != _Objects_Maximum_nodes ||
        maximum_global_objects != _Objects_MP_Maximum_global_objects
      ) {
        _MPCI_Return_packet( the_packet_prefix );

        _Internal_error( INTERNAL_ERROR_INCONSISTENT_MP_INFORMATION );
      }

      _MPCI_Return_packet( the_packet_prefix );

      break;
  }
}

/*
 *  _MPCI_Internal_packets_Send_object_was_deleted
 *
 *  This subprogram is not needed since there are no objects
 *  deleted by this manager.
 *
 */

/*
 *  _MPCI_Internal_packets_Send_extract_proxy
 *
 *  This subprogram is not needed since there are no objects
 *  deleted by this manager.
 *
 */

MPCI_Internal_packet *_MPCI_Internal_packets_Get_packet( void )
{
  return ( (MPCI_Internal_packet *) _MPCI_Get_packet() );
}

static void _MPCI_Finalize( void )
{
  if ( _System_state_Is_multiprocessing ) {
    _MPCI_Initialization();
    _MPCI_Internal_packets_Send_process_packet( MPCI_PACKETS_SYSTEM_VERIFY );
  }
}

RTEMS_SYSINIT_ITEM(
  _MPCI_Handler_early_initialization,
  RTEMS_SYSINIT_MP_EARLY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

RTEMS_SYSINIT_ITEM(
  _MPCI_Handler_initialization,
  RTEMS_SYSINIT_MP,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

RTEMS_SYSINIT_ITEM(
  _MPCI_Create_server,
  RTEMS_SYSINIT_MP_SERVER,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

RTEMS_SYSINIT_ITEM(
  _MPCI_Finalize,
  RTEMS_SYSINIT_MP_FINALIZE,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

/* end of file */
