/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the implementation of the thread queue test
 *   support.
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

#include "tx-thread-queue.h"
#include "tx-support.h"
#include "ts-config.h"

#include <rtems/score/threadimpl.h>
#include <rtems/rtems/semimpl.h>

void TQSend(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
)
{
#if defined( RTEMS_SMP )
  ctx->event_received[ worker ] = false;
#endif

  SendEvents( ctx->worker_id[ worker ], events );
}

void TQWaitForEventsReceived( const TQContext *ctx, TQWorkerKind worker )
{
#if defined( RTEMS_SMP )
  while ( !ctx->event_received[ worker ] ) {
    /* Wait */
  }
#endif
}

void TQWaitForExecutionStop( const TQContext *ctx, TQWorkerKind worker )
{
#if defined( RTEMS_SMP )
  WaitForExecutionStop( ctx->worker_id[ worker ] );
#endif
}

void TQSendAndWaitForExecutionStop(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
)
{
  TQSend( ctx, worker, events );

#if defined( RTEMS_SMP )
  TQWaitForEventsReceived( ctx, worker );
  WaitForExecutionStop( ctx->worker_id[ worker ] );
#endif
}

void TQWaitForIntendToBlock( const TQContext *ctx, TQWorkerKind worker )
{
  const rtems_tcb  *thread;
  Thread_Wait_flags intend_to_block;

  thread = ctx->worker_tcb[ worker ];
  intend_to_block = THREAD_WAIT_CLASS_OBJECT |
    THREAD_WAIT_STATE_INTEND_TO_BLOCK;

  while ( _Thread_Wait_flags_get_acquire( thread ) != intend_to_block ) {
    /* Wait */
  }
}

void TQSendAndWaitForIntendToBlock(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
)
{
  TQSend( ctx, worker, events );

#if defined( RTEMS_SMP )
  TQWaitForEventsReceived( ctx, worker );
  TQWaitForIntendToBlock( ctx, worker );
#endif
}

void TQSendAndWaitForExecutionStopOrIntendToBlock(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
)
{
#if defined( RTEMS_SMP )
  const rtems_tcb  *thread;
  Thread_Wait_flags intend_to_block;
#endif

  TQSend( ctx, worker, events );

#if defined( RTEMS_SMP )
  TQWaitForEventsReceived( ctx, worker );
  thread = ctx->worker_tcb[ worker ];
  intend_to_block = THREAD_WAIT_CLASS_OBJECT |
    THREAD_WAIT_STATE_INTEND_TO_BLOCK;

  while (
    _Thread_Is_executing_on_a_processor( thread ) &&
    _Thread_Wait_flags_get_acquire( thread ) != intend_to_block
  ) {
    /* Wait */
  }
#endif
}

void TQSendAndSynchronizeRunner(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
)
{
  T_quiet_eq_u32( QueryPendingEvents() & TQ_EVENT_RUNNER_SYNC, 0 );
  TQSend( ctx, worker, events | TQ_EVENT_RUNNER_SYNC );
  TQSynchronizeRunner();
}

void TQClearDone( TQContext *ctx, TQWorkerKind worker )
{
  ctx->done[ worker ] = false;
}

void TQWaitForDone( const TQContext *ctx, TQWorkerKind worker )
{
  while ( !ctx->done[ worker ] ) {
    /* Wait */
  }
}

void TQSynchronizeRunner( void )
{
  ReceiveAllEvents( TQ_EVENT_RUNNER_SYNC );
}

void TQSynchronizeRunner2( void )
{
  ReceiveAllEvents( TQ_EVENT_RUNNER_SYNC | TQ_EVENT_RUNNER_SYNC_2 );
}

void TQResetCounter( TQContext *ctx )
{
  ctx->counter = 0;
  memset( &ctx->worker_counter, 0, sizeof( ctx->worker_counter ) );
}

uint32_t TQGetCounter( const TQContext *ctx )
{
  return ctx->counter;
}

uint32_t TQGetWorkerCounter( const TQContext *ctx, TQWorkerKind worker )
{
  return ctx->worker_counter[ worker ];
}

void TQMutexObtain( const TQContext *ctx, TQMutex mutex )
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(
    ctx->mutex_id[ mutex ],
    RTEMS_WAIT,
    RTEMS_NO_TIMEOUT
  );
  T_rsc_success( sc );
}

void TQMutexRelease( const TQContext *ctx, TQMutex mutex )
{
  rtems_status_code sc;

  sc = rtems_semaphore_release( ctx->mutex_id[ mutex ] );
  T_rsc_success( sc );
}

void TQSetPriority(
  const TQContext *ctx,
  TQWorkerKind     worker,
  Priority         priority
)
{
  SetPriority( ctx->worker_id[ worker ], priority );
}

Priority TQGetPriority( const TQContext *ctx, TQWorkerKind worker )
{
  return GetPriority( ctx->worker_id[ worker ] );
}

void TQSetScheduler(
  const TQContext *ctx,
  TQWorkerKind     worker,
  rtems_id         scheduler_id,
  Priority         priority
)
{
#if defined( RTEMS_SMP )
  rtems_status_code sc;

  sc = rtems_task_set_scheduler(
    ctx->worker_id[ worker ],
    scheduler_id,
    priority
  );
  T_rsc_success( sc );
#else
  (void) scheduler_id;
  SetPriority( ctx->worker_id[ worker ], priority );
#endif
}

static void Count( TQContext *ctx, TQWorkerKind worker )
{
  unsigned int counter;

  counter = _Atomic_Fetch_add_uint( &ctx->counter, 1, ATOMIC_ORDER_RELAXED );
  ctx->worker_counter[ worker ] = counter + 1;
}

static void Enqueue( TQContext *ctx, TQWorkerKind worker, TQWait wait )
{
  ctx->status[ worker ] = TQEnqueue( ctx, wait );
  Count( ctx, worker );
}

static void ThreadQueueDeadlock(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  TQContext *ctx;

  ctx = arg;
  T_eq_int( source, INTERNAL_ERROR_CORE );
  T_eq_int( code, INTERNAL_ERROR_THREAD_QUEUE_DEADLOCK );
  SetFatalHandler( NULL, NULL );
  longjmp( ctx->before_enqueue, 1 );
}

static void Worker( rtems_task_argument arg, TQWorkerKind worker )
{
  TQContext *ctx;

  ctx = (TQContext *) arg;

  while ( true ) {
    rtems_event_set events;

    events = ReceiveAnyEvents();
    ctx->event_received[ worker ] = true;

    if ( ( events & TQ_EVENT_HELPER_A_SYNC ) != 0 ) {
      SendEvents( ctx->worker_id[ TQ_HELPER_A ], TQ_EVENT_RUNNER_SYNC );
    }

    if ( ( events & TQ_EVENT_HELPER_B_SYNC ) != 0 ) {
      SendEvents( ctx->worker_id[ TQ_HELPER_B ], TQ_EVENT_RUNNER_SYNC );
    }

    if ( ( events & TQ_EVENT_SCHEDULER_RECORD_START ) != 0 ) {
      TQSchedulerRecordStart( ctx );
    }

    if ( ( events & TQ_EVENT_ENQUEUE_PREPARE ) != 0 ) {
      TQEnqueuePrepare( ctx );
    }

    if ( ( events & TQ_EVENT_ENQUEUE ) != 0 ) {
      Enqueue( ctx, worker, ctx->wait );
    }

    if ( ( events & TQ_EVENT_ENQUEUE_TIMED ) != 0 ) {
      Enqueue( ctx, worker, TQ_WAIT_TIMED );
    }

    if ( ( events & TQ_EVENT_ENQUEUE_FATAL ) != 0 ) {
      SetFatalHandler( ThreadQueueDeadlock, ctx );

      if ( setjmp( ctx->before_enqueue ) == 0 ) {
        ctx->status[ worker ] = STATUS_MINUS_ONE;
        Enqueue( ctx, worker, ctx->wait );
      } else {
        ctx->status[ worker ] = STATUS_DEADLOCK;
      }
    }

    if ( ( events & TQ_EVENT_TIMEOUT ) != 0 ) {
      Per_CPU_Control *cpu_self;

      cpu_self = _Thread_Dispatch_disable();
      _Thread_Timeout( &ctx->worker_tcb[ worker ]->Timer.Watchdog );
      _Thread_Dispatch_direct( cpu_self );
    }

    if ( ( events & TQ_EVENT_FLUSH_ALL ) != 0 ) {
      TQFlush( ctx, true );
    }

    if ( ( events & TQ_EVENT_FLUSH_PARTIAL ) != 0 ) {
      TQFlush( ctx, false );
    }

    if ( ( events & TQ_EVENT_ENQUEUE_DONE ) != 0 ) {
      TQEnqueueDone( ctx );
    }

    if ( ( events & TQ_EVENT_SURRENDER ) != 0 ) {
      Status_Control status;

      status = TQSurrender( ctx );
      T_eq_int( status, TQConvertStatus( ctx, STATUS_SUCCESSFUL ) );
    }

    if ( ( events & TQ_EVENT_MUTEX_A_OBTAIN ) != 0 ) {
      TQMutexObtain( ctx, TQ_MUTEX_A );
    }

    if ( ( events & TQ_EVENT_MUTEX_A_RELEASE ) != 0 ) {
      TQMutexRelease( ctx, TQ_MUTEX_A );
    }

    if ( ( events & TQ_EVENT_MUTEX_B_OBTAIN ) != 0 ) {
      TQMutexObtain( ctx, TQ_MUTEX_B );
    }

    if ( ( events & TQ_EVENT_MUTEX_B_RELEASE ) != 0 ) {
      TQMutexRelease( ctx, TQ_MUTEX_B );
    }

    if ( ( events & TQ_EVENT_MUTEX_C_OBTAIN ) != 0 ) {
      TQMutexObtain( ctx, TQ_MUTEX_C );
    }

    if ( ( events & TQ_EVENT_MUTEX_C_RELEASE ) != 0 ) {
      TQMutexRelease( ctx, TQ_MUTEX_C );
    }

    if ( ( events & TQ_EVENT_MUTEX_D_OBTAIN ) != 0 ) {
      TQMutexObtain( ctx, TQ_MUTEX_D );
    }

    if ( ( events & TQ_EVENT_MUTEX_D_RELEASE ) != 0 ) {
      TQMutexRelease( ctx, TQ_MUTEX_D );
    }

    if ( ( events & TQ_EVENT_MUTEX_NO_PROTOCOL_OBTAIN ) != 0 ) {
      TQMutexObtain( ctx, TQ_MUTEX_NO_PROTOCOL );
    }

    if ( ( events & TQ_EVENT_MUTEX_NO_PROTOCOL_RELEASE ) != 0 ) {
      TQMutexRelease( ctx, TQ_MUTEX_NO_PROTOCOL );
    }

    if ( ( events & TQ_EVENT_MUTEX_FIFO_OBTAIN ) != 0 ) {
      TQMutexObtain( ctx, TQ_MUTEX_FIFO );
    }

    if ( ( events & TQ_EVENT_MUTEX_FIFO_RELEASE ) != 0 ) {
      TQMutexRelease( ctx, TQ_MUTEX_FIFO );
    }

    if ( ( events & TQ_EVENT_PIN ) != 0 ) {
      _Thread_Pin( _Thread_Get_executing() );
    }

    if ( ( events & TQ_EVENT_UNPIN ) != 0 ) {
      Per_CPU_Control *cpu_self;

       cpu_self = _Thread_Dispatch_disable();
      _Thread_Unpin( _Thread_Get_executing(), cpu_self );
      _Thread_Dispatch_direct( cpu_self );
    }

    if ( ( events & TQ_EVENT_SCHEDULER_RECORD_STOP ) != 0 ) {
      TQSchedulerRecordStop( ctx );
    }

    if ( ( events & TQ_EVENT_RUNNER_SYNC ) != 0 ) {
      SendEvents( ctx->runner_id, TQ_EVENT_RUNNER_SYNC );
    }

    if ( ( events & TQ_EVENT_COUNT ) != 0 ) {
      Count( ctx, worker );
    }

    if ( ( events & TQ_EVENT_BUSY_WAIT ) != 0 ) {
      while ( ctx->busy_wait[ worker ] ) {
        /* Wait */
      }
    }

    if ( ( events & TQ_EVENT_RUNNER_SYNC_2 ) != 0 ) {
      SendEvents( ctx->runner_id, TQ_EVENT_RUNNER_SYNC_2 );
    }

    ctx->done[ worker ] = true;
  }
}

static void BlockerA( rtems_task_argument arg )
{
  Worker( arg, TQ_BLOCKER_A );
}

static void BlockerB( rtems_task_argument arg )
{
  Worker( arg, TQ_BLOCKER_B );
}

static void BlockerC( rtems_task_argument arg )
{
  Worker( arg, TQ_BLOCKER_C );
}

static void BlockerD( rtems_task_argument arg )
{
  Worker( arg, TQ_BLOCKER_D );
}

static void BlockerE( rtems_task_argument arg )
{
  Worker( arg, TQ_BLOCKER_E );
}

static void WorkerF( rtems_task_argument arg )
{
  Worker( arg, TQ_WORKER_F );
}

static void HelperA( rtems_task_argument arg )
{
  Worker( arg, TQ_HELPER_A );
}

static void HelperB( rtems_task_argument arg )
{
  Worker( arg, TQ_HELPER_B );
}

static void HelperC( rtems_task_argument arg )
{
  Worker( arg, TQ_HELPER_C );
}

void TQInitialize( TQContext *ctx )
{
  rtems_status_code sc;
  size_t            i;

  ctx->runner_id = rtems_task_self();
  ctx->runner_tcb = GetThread( RTEMS_SELF );

  /*
   * Use a lower priority than all started worker tasks to make sure they wait
   * for events.
   */
  SetSelfPriority( PRIO_VERY_LOW );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->mutex_id ); ++i ) {
    rtems_attribute attributes;

    attributes = RTEMS_BINARY_SEMAPHORE;

    if ( i == TQ_MUTEX_NO_PROTOCOL ) {
      attributes |= RTEMS_PRIORITY;
    } else if ( i == TQ_MUTEX_FIFO ) {
      attributes |= RTEMS_FIFO;
    } else {
      attributes |= RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY;
    }

    sc = rtems_semaphore_create(
      rtems_build_name( 'M', 'T', 'X', 'A' + i ),
      1,
      attributes,
      0,
      &ctx->mutex_id[ i ]
    );
    T_rsc_success( sc );
  }

  ctx->worker_id[ TQ_BLOCKER_A ] = CreateTask( "BLKA", PRIO_HIGH );
  StartTask( ctx->worker_id[ TQ_BLOCKER_A ], BlockerA, ctx );
  ctx->worker_id[ TQ_BLOCKER_B ] = CreateTask( "BLKB", PRIO_VERY_HIGH );
  StartTask( ctx->worker_id[ TQ_BLOCKER_B ], BlockerB, ctx );
  ctx->worker_id[ TQ_BLOCKER_C ] = CreateTask( "BLKC", PRIO_ULTRA_HIGH );
  StartTask( ctx->worker_id[ TQ_BLOCKER_C ], BlockerC, ctx );
  ctx->worker_id[ TQ_BLOCKER_D ] = CreateTask( "BLKD", PRIO_LOW );
  StartTask( ctx->worker_id[ TQ_BLOCKER_D ], BlockerD, ctx );
  ctx->worker_id[ TQ_BLOCKER_E ] = CreateTask( "BLKE", PRIO_LOW );
  StartTask( ctx->worker_id[ TQ_BLOCKER_E ], BlockerE, ctx );
  ctx->worker_id[ TQ_WORKER_F ] = CreateTask( "WRKF", PRIO_LOW );
  StartTask( ctx->worker_id[ TQ_WORKER_F ], WorkerF, ctx );
  ctx->worker_id[ TQ_HELPER_A ] = CreateTask( "HLPA", PRIO_LOW );
  StartTask( ctx->worker_id[ TQ_HELPER_A ], HelperA, ctx );
  ctx->worker_id[ TQ_HELPER_B ] = CreateTask( "HLPB", PRIO_LOW );
  StartTask( ctx->worker_id[ TQ_HELPER_B ], HelperB, ctx );
  ctx->worker_id[ TQ_HELPER_C ] = CreateTask( "HLPC", PRIO_LOW );
  StartTask( ctx->worker_id[ TQ_HELPER_C ], HelperC, ctx );

  for (i = 0; i < RTEMS_ARRAY_SIZE( ctx->worker_tcb ); ++i) {
    ctx->worker_tcb[ i ] = GetThread( ctx->worker_id[ i ] );
  }

  SetSelfPriority( PRIO_NORMAL );
}

void TQDestroy( TQContext *ctx )
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->worker_id ); ++i ) {
    DeleteTask( ctx->worker_id[ i ] );
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->mutex_id ); ++i ) {
    if ( ctx->mutex_id[ i ] != 0 ) {
      rtems_status_code sc;

      sc = rtems_semaphore_delete( ctx->mutex_id[ i ] );
      T_rsc_success( sc );
    }
  }

  RestoreRunnerPriority();
}

void TQReset( TQContext *ctx )
{
  rtems_id scheduler_id;

  scheduler_id = SCHEDULER_A_ID;
  SetScheduler( ctx->runner_id, scheduler_id, PRIO_NORMAL );
  TQSetScheduler( ctx, TQ_BLOCKER_A, scheduler_id, PRIO_HIGH );
  TQSetScheduler( ctx, TQ_BLOCKER_B, scheduler_id, PRIO_VERY_HIGH );
  TQSetScheduler( ctx, TQ_BLOCKER_C, scheduler_id, PRIO_ULTRA_HIGH );
  TQSetScheduler( ctx, TQ_BLOCKER_D, scheduler_id, PRIO_LOW );
  TQSetScheduler( ctx, TQ_BLOCKER_E, scheduler_id, PRIO_LOW );
  TQSetScheduler( ctx, TQ_HELPER_A, scheduler_id, PRIO_LOW );
  TQSetScheduler( ctx, TQ_HELPER_B, scheduler_id, PRIO_LOW );
  TQSetScheduler( ctx, TQ_HELPER_C, scheduler_id, PRIO_LOW );
}

void TQSortMutexesByID( TQContext *ctx )
{
  size_t i;
  size_t n;

  n = 3;

  /* Bubble sort */
  for ( i = 1; i < n ; ++i ) {
    size_t j;

    for ( j = 0; j < n - i; ++j ) {
      if ( ctx->mutex_id[ j ] > ctx->mutex_id[ j + 1 ] ) {
       rtems_id tmp;

       tmp = ctx->mutex_id[ j ];
       ctx->mutex_id[ j ] = ctx->mutex_id[ j + 1 ];
       ctx->mutex_id[ j + 1 ] = tmp;
      }
    }
  }
}

void TQGetProperties( TQContext *ctx, TQWorkerKind enqueued_worker )
{
  ( *ctx->get_properties )( ctx, enqueued_worker );
}

Status_Control TQConvertStatus( TQContext *ctx, Status_Control status )
{
  return ( *ctx->convert_status )( status );
}

void TQEnqueuePrepare( TQContext *ctx )
{
  ( *ctx->enqueue_prepare )( ctx );
}

Status_Control TQEnqueue( TQContext *ctx, TQWait wait )
{
  return ( *ctx->enqueue )( ctx, wait );
}

Status_Control TQEnqueueFatal( TQContext *ctx )
{
  Status_Control status;

  SetFatalHandler( ThreadQueueDeadlock, ctx );
  status = STATUS_MINUS_ONE;

  if ( setjmp( ctx->before_enqueue ) == 0 ) {
    status = TQEnqueue( ctx, ctx->wait );
  } else {
    status = STATUS_DEADLOCK;
  }

  return status;
}

void TQEnqueueDone( TQContext *ctx )
{
  ( *ctx->enqueue_done )( ctx );
}

Status_Control TQSurrender( TQContext *ctx )
{
  return ( *ctx->surrender )( ctx );
}

void TQFlush( TQContext *ctx, bool flush_all )
{
  ctx->flush_count = ( *ctx->flush )( ctx, ctx->how_many, flush_all );
}

rtems_tcb *TQGetOwner( TQContext *ctx )
{
  rtems_tcb *( *get_owner )( TQContext * );

  get_owner = ctx->get_owner;

  if ( get_owner == NULL ) {
    return NULL;
  }

  return ( *get_owner )( ctx );
}

void TQSchedulerRecordStart( TQContext *ctx )
{
  T_scheduler_log *log;

  log = T_scheduler_record_40( &ctx->scheduler_log );
  T_null( log );
}

void TQSchedulerRecordStop( TQContext *ctx )
{
  T_scheduler_log *log;

  log = T_scheduler_record( NULL );
  T_eq_ptr( &log->header, &ctx->scheduler_log.header );
}

const T_scheduler_event *TQGetNextAny( TQContext *ctx, size_t *index )
{
  return T_scheduler_next_any(
    &ctx->scheduler_log.header,
    index
  );
}

const T_scheduler_event *TQGetNextBlock( TQContext *ctx, size_t *index )
{
  return T_scheduler_next(
    &ctx->scheduler_log.header,
    T_SCHEDULER_BLOCK,
    index
  );
}

const T_scheduler_event *TQGetNextUnblock( TQContext *ctx, size_t *index )
{
  return T_scheduler_next(
    &ctx->scheduler_log.header,
    T_SCHEDULER_UNBLOCK,
    index
  );
}

const T_scheduler_event *TQGetNextUpdatePriority(
  TQContext *ctx,
  size_t    *index
)
{
  return T_scheduler_next(
    &ctx->scheduler_log.header,
    T_SCHEDULER_UPDATE_PRIORITY,
    index
  );
}

const T_scheduler_event *TQGetNextAskForHelp(
  TQContext *ctx,
  size_t    *index
)
{
  return T_scheduler_next(
    &ctx->scheduler_log.header,
    T_SCHEDULER_ASK_FOR_HELP,
    index
  );
}

void TQDoNothing( TQContext *ctx )
{
  (void) ctx;
}

Status_Control TQDoNothingSuccessfully( TQContext *ctx )
{
  (void) ctx;

  return STATUS_SUCCESSFUL;
}

Status_Control TQConvertStatusClassic( Status_Control status )
{
  return STATUS_BUILD( STATUS_GET_CLASSIC( status ), 0 );
}

Status_Control TQConvertStatusPOSIX( Status_Control status )
{
  return STATUS_BUILD( 0, STATUS_GET_POSIX( status ) );
}

void TQEnqueuePrepareDefault( TQContext *ctx )
{
  Status_Control status;

  status = TQEnqueue( ctx, TQ_NO_WAIT );
  T_eq_int( status, TQConvertStatus( ctx, STATUS_SUCCESSFUL ) );
}

void TQEnqueueDoneDefault( TQContext *ctx )
{
  Status_Control status;

  status = TQSurrender( ctx );
  T_eq_int( status, TQConvertStatus( ctx, STATUS_SUCCESSFUL ) );
}

Status_Control TQEnqueueClassicSem( TQContext *ctx, TQWait wait )
{
  rtems_status_code sc;
  rtems_option      option;
  rtems_option      timeout;

  switch ( wait ) {
    case TQ_WAIT_FOREVER:
      option = RTEMS_WAIT;
      timeout = RTEMS_NO_TIMEOUT;
      break;
    case TQ_WAIT_TIMED:
      option = RTEMS_WAIT;
      timeout = UINT32_MAX;
      break;
    default:
      option = RTEMS_NO_WAIT;
      timeout = 0;
      break;
  }

  sc = rtems_semaphore_obtain( ctx->thread_queue_id, option, timeout );

  return STATUS_BUILD( sc, 0 );
}

Status_Control TQSurrenderClassicSem( TQContext *ctx )
{
  rtems_status_code sc;

  sc = rtems_semaphore_release( ctx->thread_queue_id );

  return STATUS_BUILD( sc, 0 );
}

rtems_tcb *TQGetOwnerClassicSem( TQContext *ctx )
{
  Semaphore_Control   *semaphore;
  Thread_queue_Context queue_context;
  rtems_tcb           *thread;

  semaphore = _Semaphore_Get( ctx->thread_queue_id, &queue_context );
  T_assert_not_null( semaphore );
  thread = semaphore->Core_control.Wait_queue.Queue.owner;
  _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );

  return thread;
}

uint32_t TQSemGetCount( TQSemContext *ctx )
{
  return ( *ctx->get_count )( ctx );
}

void TQSemSetCount( TQSemContext *ctx, uint32_t count )
{
  ( *ctx->set_count )( ctx, count );
}

uint32_t TQSemGetCountClassic( TQSemContext *ctx )
{
  Semaphore_Control   *semaphore;
  Thread_queue_Context queue_context;
  uint32_t             count;

  semaphore = _Semaphore_Get( ctx->base.thread_queue_id, &queue_context );
  T_assert_not_null( semaphore );
  count = semaphore->Core_control.Semaphore.count;
  _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );

  return count;
}

void TQSemSetCountClassic( TQSemContext *ctx, uint32_t count )
{
  Semaphore_Control   *semaphore;
  Thread_queue_Context queue_context;

  semaphore = _Semaphore_Get( ctx->base.thread_queue_id, &queue_context );
  T_assert_not_null( semaphore );
  semaphore->Core_control.Semaphore.count = count;
  _ISR_lock_ISR_enable( &queue_context.Lock_context.Lock_context );
}
