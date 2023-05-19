/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the implementation of support functions for
 *   the validation test cases.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include "tx-support.h"
#include "ts-config.h"

#include <rtems/test.h>
#include <rtems/score/percpu.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>
#include <rtems/rtems/semimpl.h>

#include <string.h>

rtems_id DoCreateTask( rtems_name name, rtems_task_priority priority )
{
  rtems_status_code sc;
  rtems_id          id;

  sc = rtems_task_create(
    name,
    priority,
    TEST_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  T_assert_rsc_success( sc );

  return id;
}

void StartTask( rtems_id id, rtems_task_entry entry, void *arg )
{
  rtems_status_code sc;

  sc = rtems_task_start( id, entry, (rtems_task_argument) arg);
  T_assert_rsc_success( sc );
}

void DeleteTask( rtems_id id )
{
  if ( id != 0 ) {
    rtems_status_code sc;

    sc = rtems_task_delete( id );
    T_quiet_rsc_success( sc );
  }
}

void SuspendTask( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_task_suspend( id );
  T_quiet_rsc_success( sc );
}

void SuspendSelf( void )
{
  SuspendTask( RTEMS_SELF );
}

void ResumeTask( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_task_resume( id );
  T_quiet_rsc_success( sc );
}

bool IsTaskSuspended( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_task_is_suspended( id );
  T_quiet_true( sc == RTEMS_SUCCESSFUL || sc == RTEMS_ALREADY_SUSPENDED );

  return sc == RTEMS_ALREADY_SUSPENDED;
}

rtems_event_set QueryPendingEvents( void )
{
  rtems_status_code sc;
  rtems_event_set   events;

  events = 0;
  sc = rtems_event_receive(
    RTEMS_PENDING_EVENTS,
    RTEMS_EVENT_ALL | RTEMS_NO_WAIT,
    0,
    &events
  );
  T_quiet_rsc_success( sc );

  return events;
}

rtems_event_set PollAnyEvents( void )
{
  rtems_event_set events;

  events = 0;
  (void) rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_EVENT_ANY | RTEMS_NO_WAIT,
    0,
    &events
  );

  return events;
}

rtems_event_set ReceiveAnyEvents( void )
{
  return ReceiveAnyEventsTimed( RTEMS_NO_TIMEOUT );
}

rtems_event_set ReceiveAnyEventsTimed( rtems_interval ticks )
{
  rtems_event_set events;

  events = 0;
  (void) rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_EVENT_ANY | RTEMS_WAIT,
    ticks,
    &events
  );

  return events;
}

void ReceiveAllEvents( rtems_event_set events )
{
  rtems_status_code sc;
  rtems_event_set   received;

  received = 0;
  sc = rtems_event_receive(
    events,
    RTEMS_EVENT_ALL | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &received
  );
  T_quiet_rsc_success( sc );
  T_quiet_eq_u32( received, events );
}

void SendEvents( rtems_id id, rtems_event_set events )
{
  rtems_status_code sc;

  sc = rtems_event_send( id, events );
  T_quiet_rsc_success( sc );
}

rtems_mode GetMode( void )
{
  return SetMode( RTEMS_DEFAULT_MODES, RTEMS_CURRENT_MODE );
}

rtems_mode SetMode( rtems_mode set, rtems_mode mask )
{
  rtems_status_code sc;
  rtems_mode        previous;

  sc = rtems_task_mode( set, mask, &previous );
  T_quiet_rsc_success( sc );

  return previous;
}

rtems_task_priority GetPriority( rtems_id id )
{
  return SetPriority( id, RTEMS_CURRENT_PRIORITY );
}

rtems_task_priority GetPriorityByScheduler(
  rtems_id task_id,
  rtems_id scheduler_id
)
{
  rtems_status_code   sc;
  rtems_task_priority priority;

  priority = PRIO_INVALID;
  sc = rtems_task_get_priority( task_id, scheduler_id, &priority );

  if ( sc != RTEMS_SUCCESSFUL ) {
    return PRIO_INVALID;
  }

  return priority;
}

rtems_task_priority SetPriority( rtems_id id, rtems_task_priority priority )
{
  rtems_status_code   sc;
  rtems_task_priority previous;

  previous = PRIO_INVALID;
  sc = rtems_task_set_priority( id, priority, &previous );
  T_quiet_rsc_success( sc );

  return previous;
}

rtems_task_priority GetSelfPriority( void )
{
  return SetPriority( RTEMS_SELF, RTEMS_CURRENT_PRIORITY );
}

rtems_task_priority SetSelfPriority( rtems_task_priority priority )
{
  return SetPriority( RTEMS_SELF, priority );
}

rtems_task_priority SetSelfPriorityNoYield( rtems_task_priority priority )
{
  rtems_status_code sc;
  rtems_id          id;

  /*
   * If the priority is lowered, then this sequence ensures that we do not
   * carry out an implicit yield.
   */

  sc = rtems_semaphore_create(
    rtems_build_name( 'T', 'E', 'M', 'P' ),
    0,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_PRIORITY_CEILING,
    1,
    &id
  );
  T_quiet_rsc_success( sc );

  priority = SetSelfPriority( priority );
  ReleaseMutex( id );
  DeleteMutex( id );

  return priority;
}

rtems_id GetScheduler( rtems_id id )
{
  rtems_status_code sc;
  rtems_id          scheduler_id;

  scheduler_id = 0xffffffff;
  sc = rtems_task_get_scheduler( id, &scheduler_id );
  T_quiet_rsc_success( sc );

  return scheduler_id;
}

rtems_id GetSelfScheduler( void )
{
  return GetScheduler( RTEMS_SELF );
}

void SetScheduler(
  rtems_id            task_id,
  rtems_id            scheduler_id,
  rtems_task_priority priority
)
{
  rtems_status_code sc;

  sc = rtems_task_set_scheduler( task_id, scheduler_id, priority );
  T_quiet_rsc_success( sc );
}

void SetSelfScheduler( rtems_id scheduler_id, rtems_task_priority priority )
{
  SetScheduler( RTEMS_SELF, scheduler_id, priority );
}

void GetAffinity( rtems_id id, cpu_set_t *set )
{
  rtems_status_code sc;

  CPU_ZERO( set );
  sc = rtems_task_get_affinity( id, sizeof( *set ), set );
  T_quiet_rsc_success( sc );
}

void GetSelfAffinity( cpu_set_t *set )
{
  GetAffinity( RTEMS_SELF, set );
}

void SetAffinity( rtems_id id, const cpu_set_t *set )
{
  rtems_status_code sc;

  sc = rtems_task_set_affinity( id, sizeof( *set ), set );
  T_quiet_rsc_success( sc );
}

void SetSelfAffinity( const cpu_set_t *set )
{
  SetAffinity( RTEMS_SELF, set );
}

void SetAffinityOne( rtems_id id, uint32_t cpu_index )
{
  cpu_set_t set;

  CPU_ZERO( &set );
  CPU_SET( (int) cpu_index, &set );
  SetAffinity( id, &set );
}

void SetSelfAffinityOne( uint32_t cpu_index )
{
  SetAffinityOne( RTEMS_SELF, cpu_index );
}

void SetAffinityAll( rtems_id id )
{
  cpu_set_t set;

  CPU_FILL( &set );
  SetAffinity( id, &set );
}

void SetSelfAffinityAll( void )
{
  SetAffinityAll( RTEMS_SELF );
}

void Yield( void )
{
  rtems_status_code sc;

  sc = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  T_quiet_rsc_success( sc );
}

void YieldTask( rtems_id id )
{
  Thread_Control  *the_thread;
  ISR_lock_Context lock_context;
  Per_CPU_Control *cpu_self;

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
    return;
  }

  cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
  _ISR_lock_ISR_enable( &lock_context);
  _Thread_Yield( the_thread );
  _Thread_Dispatch_direct( cpu_self );
}

void AddProcessor( rtems_id scheduler_id, uint32_t cpu_index )
{
  rtems_status_code sc;

  sc = rtems_scheduler_add_processor( scheduler_id, cpu_index );
  T_quiet_rsc_success( sc );
}

void RemoveProcessor( rtems_id scheduler_id, uint32_t cpu_index )
{
  rtems_status_code sc;

  sc = rtems_scheduler_remove_processor( scheduler_id, cpu_index );
  T_quiet_rsc_success( sc );
}

rtems_id CreateMutex( void )
{
  rtems_status_code sc;
  rtems_id          id;

  id = INVALID_ID;
  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'U', 'T', 'X' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    0,
    &id
  );
  T_rsc_success( sc );

  return id;
}

rtems_id CreateMutexNoProtocol( void )
{
  rtems_status_code sc;
  rtems_id          id;

  id = INVALID_ID;
  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'U', 'T', 'X' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY,
    0,
    &id
  );
  T_rsc_success( sc );

  return id;
}

rtems_id CreateMutexFIFO( void )
{
  rtems_status_code sc;
  rtems_id          id;

  id = INVALID_ID;
  sc = rtems_semaphore_create(
    rtems_build_name( 'M', 'U', 'T', 'X' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_FIFO,
    0,
    &id
  );
  T_rsc_success( sc );

  return id;
}

void DeleteMutex( rtems_id id )
{
  if ( id != INVALID_ID ) {
    rtems_status_code sc;

    sc = rtems_semaphore_delete( id );
    T_rsc_success( sc );
  }
}

bool IsMutexOwner( rtems_id id )
{
  Semaphore_Control   *the_semaphore;
  Thread_queue_Context queue_context;

  the_semaphore = _Semaphore_Get( id, &queue_context );
  if ( the_semaphore == NULL ) {
    return false;
  }

  _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );
  return the_semaphore->Core_control.Wait_queue.Queue.owner ==
    _Thread_Get_executing();
}

void ObtainMutex( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain( id, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  T_rsc_success( sc );
}

void ObtainMutexTimed( rtems_id id, rtems_interval ticks )
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain( id, RTEMS_WAIT, ticks );
  T_rsc_success( sc );
}

void ObtainMutexDeadlock( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain( id, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  T_rsc( sc, RTEMS_INCORRECT_STATE );
}

void ReleaseMutex( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_semaphore_release( id );
  T_rsc_success( sc );
}

Thread_queue_Queue *GetMutexThreadQueue( rtems_id id )
{
  Semaphore_Control   *the_semaphore;
  Thread_queue_Context queue_context;

  the_semaphore = _Semaphore_Get( id, &queue_context );
  if ( the_semaphore == NULL ) {
    return NULL;
  }

  _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );
  return &the_semaphore->Core_control.Wait_queue.Queue;
}

void RestoreRunnerASR( void )
{
  rtems_status_code sc;

  sc = rtems_signal_catch( NULL, RTEMS_DEFAULT_MODES );
  T_quiet_rsc_success( sc );
}

void RestoreRunnerMode( void )
{
  rtems_status_code sc;
  rtems_mode        mode;

  sc = rtems_task_mode( RTEMS_DEFAULT_MODES, RTEMS_ALL_MODE_MASKS, &mode );
  T_quiet_rsc_success( sc );
}

void RestoreRunnerPriority( void )
{
  SetSelfPriority( 1 );
}

void RestoreRunnerScheduler( void )
{
  SetSelfScheduler( SCHEDULER_A_ID, 1 );
}

Thread_Control *GetThread( rtems_id id )
{
  Thread_Control  *the_thread;
  ISR_lock_Context lock_context;

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
    return NULL;
  }

  _ISR_lock_ISR_enable( &lock_context);
  return the_thread;
}

Thread_Control *GetExecuting( void )
{
  return _Thread_Get_executing();
}

void KillZombies( void )
{
  _RTEMS_Lock_allocator();
  _Thread_Kill_zombies();
  _RTEMS_Unlock_allocator();
}

void WaitForExecutionStop( rtems_id task_id )
{
#if defined( RTEMS_SMP )
  Thread_Control *the_thread;

  the_thread = GetThread( task_id );
  T_assert_not_null( the_thread );

  while ( _Thread_Is_executing_on_a_processor( the_thread ) ) {
    /* Wait */
  }
#else
  (void) task_id;
#endif
}

void WaitForIntendToBlock( rtems_id task_id )
{
#if defined( RTEMS_SMP )
  Thread_Control   *the_thread;
  Thread_Wait_flags intend_to_block;

  the_thread = GetThread( task_id );
  T_assert_not_null( the_thread );

  intend_to_block = THREAD_WAIT_CLASS_OBJECT |
    THREAD_WAIT_STATE_INTEND_TO_BLOCK;

  while ( _Thread_Wait_flags_get_acquire( the_thread ) != intend_to_block ) {
    /* Wait */
  }
#else
  (void) task_id;
#endif
}

void WaitForHeir( uint32_t cpu_index, rtems_id task_id )
{
  Per_CPU_Control *cpu;

  cpu = _Per_CPU_Get_by_index( cpu_index );

  while ( cpu->heir->Object.id != task_id ) {
    RTEMS_COMPILER_MEMORY_BARRIER();
  }
}

void WaitForNextTask( uint32_t cpu_index, rtems_id task_id )
{
  Per_CPU_Control *cpu;

  cpu = _Per_CPU_Get_by_index( cpu_index );

  while ( cpu->heir->Object.id == task_id ) {
    RTEMS_COMPILER_MEMORY_BARRIER();
  }

  while ( cpu->thread_dispatch_disable_level != 0 ) {
    RTEMS_COMPILER_MEMORY_BARRIER();
  }
}

void GetTaskTimerInfo( rtems_id id, TaskTimerInfo *info )
{
  GetTaskTimerInfoByThread( GetThread( id ), info );
}

void GetTaskTimerInfoByThread(
  struct _Thread_Control *thread,
  TaskTimerInfo          *info
)
{
  info->expire_ticks = 0;
  info->expire_timespec.tv_sec = -1;
  info->expire_timespec.tv_nsec = -1;

  if ( thread != NULL ) {
    ISR_lock_Context lock_context;
    ISR_lock_Context lock_context_2;
    Per_CPU_Control *cpu;

    _ISR_lock_ISR_disable_and_acquire( &thread->Timer.Lock, &lock_context );
    info->expire_ticks = thread->Timer.Watchdog.expire;
#if defined( RTEMS_SMP )
    cpu = thread->Timer.Watchdog.cpu;
#else
    cpu = _Per_CPU_Get();
#endif
    _Watchdog_Per_CPU_acquire_critical( cpu, &lock_context_2 );

    if ( _Watchdog_Is_scheduled( &thread->Timer.Watchdog ) ) {
      const Watchdog_Header *hdr;

      hdr = thread->Timer.header;

      if ( hdr == &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ] ) {
        info->state = TASK_TIMER_TICKS;
      } else {
        _Watchdog_Ticks_to_timespec(
          info->expire_ticks,
          &info->expire_timespec
        );

        if ( hdr == &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_REALTIME ] ) {
          info->state = TASK_TIMER_REALTIME;
        } else {
          T_quiet_eq_ptr(
            hdr,
            &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_MONOTONIC ]
          );
          info->state = TASK_TIMER_MONOTONIC;
        }
      }
    } else {
      info->state = TASK_TIMER_INACTIVE;
    }

    _Watchdog_Per_CPU_release_critical( cpu, &lock_context_2 );
    _ISR_lock_Release_and_ISR_enable( &thread->Timer.Lock, &lock_context );
  } else {
    info->state = TASK_TIMER_INVALID;
  }
}

#if defined( RTEMS_SMP )
static void DoWatchdogTick( void *arg )
{
  (void) arg;
  _Watchdog_Tick( _Per_CPU_Get() );
}
#endif

void ClockTick( void )
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
#if defined( RTEMS_SMP )
  DoWatchdogTick( NULL );
  _SMP_Othercast_action( DoWatchdogTick, NULL );
#else
  _Watchdog_Tick( cpu_self );
#endif
  _Thread_Dispatch_enable( cpu_self );
}

static void FinalWatchdogTick( Per_CPU_Control *cpu )
{
  ISR_lock_Context  lock_context;
  Watchdog_Header  *header;
  Watchdog_Control *first;

  _ISR_lock_ISR_disable_and_acquire( &cpu->Watchdog.Lock, &lock_context );

  header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ];
  first = _Watchdog_Header_first( header );

  if ( first != NULL ) {
    _Watchdog_Tickle(
      header,
      first,
      UINT64_MAX,
      &cpu->Watchdog.Lock,
      &lock_context
    );
  }

  header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_MONOTONIC ];
  first = _Watchdog_Header_first( header );

  if ( first != NULL ) {
    _Watchdog_Tickle(
      header,
      first,
      UINT64_MAX,
      &cpu->Watchdog.Lock,
      &lock_context
    );
  }

  header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_REALTIME ];
  first = _Watchdog_Header_first( header );

  if ( first != NULL ) {
    _Watchdog_Tickle(
      header,
      first,
      UINT64_MAX,
      &cpu->Watchdog.Lock,
      &lock_context
    );
  }

  _ISR_lock_Release_and_ISR_enable( &cpu->Watchdog.Lock, &lock_context );
}

#if defined( RTEMS_SMP )
static void DoFinalWatchdogTick( void *arg )
{
  (void) arg;
  FinalWatchdogTick( _Per_CPU_Get() );
}
#endif

void FinalClockTick( void )
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
#if defined( RTEMS_SMP )
  DoFinalWatchdogTick( NULL );
  _SMP_Othercast_action( DoFinalWatchdogTick, NULL );
#else
  FinalWatchdogTick( cpu_self );
#endif
  _Thread_Dispatch_enable( cpu_self );
}

static FatalHandler fatal_handler;

static void *fatal_arg;

void FatalInitialExtension(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
)
{
  FatalHandler fatal;

  T_quiet_false( always_set_to_false );
  fatal = fatal_handler;

  if ( fatal != NULL ) {
    ( *fatal )( source, code, fatal_arg );
  }
}

void SetFatalHandler( FatalHandler fatal, void *arg )
{
  fatal_handler = fatal;
  fatal_arg = arg;
}

static rtems_id task_switch_id;

static rtems_task_switch_extension task_switch_extension;

static void TaskSwitchExtension( rtems_tcb *executing, rtems_tcb *heir )
{
  ( *task_switch_extension )( executing, heir );
}

void SetTaskSwitchExtension( rtems_task_switch_extension task_switch )
{
  rtems_task_switch_extension last;
  rtems_status_code           sc;

  last = task_switch_extension;

  if ( task_switch == NULL ) {
    if ( last != NULL ) {
      sc = rtems_extension_delete( task_switch_id );
      T_quiet_rsc_success( sc );

      task_switch_extension = NULL;
    }
  } else {
    task_switch_extension = task_switch;

    if ( last == NULL ) {
      rtems_extensions_table table = {
        .thread_switch = TaskSwitchExtension
      };

      sc = rtems_extension_create(
        rtems_build_name( 'T', 'S', 'W', 'I' ),
        &table,
        &task_switch_id
      );
      T_quiet_rsc_success( sc );
    }
  }
}

void ClearExtensionCalls( ExtensionCalls *calls )
{
  memset( calls, 0, sizeof( *calls ) );
}

void CopyExtensionCalls( const ExtensionCalls *from, ExtensionCalls *to )
{
  memcpy( to, from, sizeof( *to ) );
}

#if defined(RTEMS_SMP)
static volatile bool delay_thread_dispatch;

static void DelayThreadDispatchHandler( void *arg )
{
  (void) arg;

  while ( delay_thread_dispatch ) {
    /* Wait */
  }
}

static const Per_CPU_Job_context delay_thread_dispatch_context = {
  .handler = DelayThreadDispatchHandler
};

static Per_CPU_Job delay_thread_dispatch_job = {
  .context = &delay_thread_dispatch_context
};
#endif

void StartDelayThreadDispatch( uint32_t cpu_index )
{
#if defined(RTEMS_SMP)
  if ( rtems_configuration_get_maximum_processors() > cpu_index ) {
    delay_thread_dispatch = true;
    _Per_CPU_Submit_job(
      _Per_CPU_Get_by_index( cpu_index ),
      &delay_thread_dispatch_job
    );
  }
#endif
}

void StopDelayThreadDispatch( uint32_t cpu_index )
{
#if defined(RTEMS_SMP)
  if ( rtems_configuration_get_maximum_processors() > cpu_index ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable();
    delay_thread_dispatch = false;
    _Per_CPU_Wait_for_job(
      _Per_CPU_Get_by_index( cpu_index ),
      &delay_thread_dispatch_job
    );
    _Thread_Dispatch_enable( cpu_self );
  }
#endif
}

bool AreInterruptsEnabled( void )
{
  return _ISR_Get_level() == 0;
}

static bool IsWhiteSpace( char c )
{
  return c == ' ' || c == '\t';
}

bool IsWhiteSpaceOnly( const char *s )
{
  char c;

  while ( ( c = *s ) != '\0' ) {
    if ( !IsWhiteSpace( c ) ) {
      return false;
    }

    ++s;
  }

  return true;
}

static const char *EatWhiteSpace( const char *s )
{
  char c;

  while ( ( c = *s ) != '\0' ) {
    if ( !IsWhiteSpace( c ) ) {
      break;
    }

    ++s;
  }

  return s;
}

bool IsEqualIgnoreWhiteSpace( const char *a, const char *b )
{
  while ( true ) {
    a = EatWhiteSpace( a );
    b = EatWhiteSpace( b );

    if ( *a != *b ) {
      return false;
    }

    if ( *a == '\0' ) {
      return true;
    }

    ++a;
    ++b;
  }

  return true;
}

#if defined(RTEMS_SMP)
bool TicketLockIsAvailable( const SMP_ticket_lock_Control *lock )
{
  unsigned int now_serving;
  unsigned int next_ticket;

  now_serving = _Atomic_Load_uint( &lock->now_serving, ATOMIC_ORDER_RELAXED );
  next_ticket = _Atomic_Load_uint( &lock->next_ticket, ATOMIC_ORDER_RELAXED );

  return now_serving == next_ticket;
}

void TicketLockWaitForOwned( const SMP_ticket_lock_Control *lock )
{
  while ( TicketLockIsAvailable( lock ) ) {
    /* Wait */
  }
}

void TicketLockWaitForOthers(
  const SMP_ticket_lock_Control *lock,
  unsigned int                   others
)
{
  unsigned int expected;
  unsigned int actual;

  expected = _Atomic_Load_uint( &lock->now_serving, ATOMIC_ORDER_RELAXED );
  expected += others + 1;

  do {
    actual = _Atomic_Load_uint( &lock->next_ticket, ATOMIC_ORDER_RELAXED );
  } while ( expected != actual );
}
#endif
