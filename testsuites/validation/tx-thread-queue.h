/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the functions to test the
 *   @ref RTEMSScoreThreadQueue.
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

#ifndef _TX_THREAD_QUEUE_H
#define _TX_THREAD_QUEUE_H

#include "tx-support.h"

#include <rtems/test-scheduler.h>
#include <rtems/score/atomic.h>
#include <rtems/score/status.h>

#include <setjmp.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSTestSuitesValidation
 *
 * @{
 */

typedef enum {
  TQ_NODE_ONLY,
  TQ_NODE_VITAL,
  TQ_NODE_DISPENSABLE
} TQNodeKind;

typedef enum {
  TQ_WAIT_STATE_BLOCKED,
  TQ_WAIT_STATE_INTEND_TO_BLOCK,
  TQ_WAIT_STATE_READY_AGAIN
} TQWaitState;

typedef enum {
  TQ_BLOCKER_A,
  TQ_BLOCKER_B,
  TQ_BLOCKER_C,
  TQ_BLOCKER_D,
  TQ_BLOCKER_E,
  TQ_WORKER_F,
  TQ_HELPER_A,
  TQ_HELPER_B,
  TQ_HELPER_C,
  TQ_WORKER_COUNT
} TQWorkerKind;

typedef enum {
  TQ_MUTEX_A,
  TQ_MUTEX_B,
  TQ_MUTEX_C,
  TQ_MUTEX_D,
  TQ_MUTEX_NO_PROTOCOL,
  TQ_MUTEX_FIFO,
  TQ_MUTEX_COUNT
} TQMutex;

typedef enum {
  TQ_FIFO,
  TQ_PRIORITY
} TQDiscipline;

typedef enum {
  TQ_NO_WAIT,
  TQ_WAIT_FOREVER,
  TQ_WAIT_TIMED
} TQWait;

typedef enum {
  TQ_DEADLOCK_STATUS,
  TQ_DEADLOCK_FATAL
} TQDeadlock;

typedef enum {
  TQ_EVENT_ENQUEUE_PREPARE = RTEMS_EVENT_0,
  TQ_EVENT_ENQUEUE = RTEMS_EVENT_1,
  TQ_EVENT_ENQUEUE_DONE = RTEMS_EVENT_2,
  TQ_EVENT_SURRENDER = RTEMS_EVENT_3,
  TQ_EVENT_RUNNER_SYNC = RTEMS_EVENT_4,
  TQ_EVENT_RUNNER_SYNC_2 = RTEMS_EVENT_5,
  TQ_EVENT_HELPER_A_SYNC = RTEMS_EVENT_6,
  TQ_EVENT_HELPER_B_SYNC = RTEMS_EVENT_7,
  TQ_EVENT_MUTEX_A_OBTAIN = RTEMS_EVENT_8,
  TQ_EVENT_MUTEX_A_RELEASE = RTEMS_EVENT_9,
  TQ_EVENT_MUTEX_B_OBTAIN = RTEMS_EVENT_10,
  TQ_EVENT_MUTEX_B_RELEASE = RTEMS_EVENT_11,
  TQ_EVENT_BUSY_WAIT = RTEMS_EVENT_12,
  TQ_EVENT_FLUSH_ALL = RTEMS_EVENT_13,
  TQ_EVENT_FLUSH_PARTIAL = RTEMS_EVENT_14,
  TQ_EVENT_SCHEDULER_RECORD_START = RTEMS_EVENT_15,
  TQ_EVENT_SCHEDULER_RECORD_STOP = RTEMS_EVENT_16,
  TQ_EVENT_TIMEOUT = RTEMS_EVENT_17,
  TQ_EVENT_MUTEX_NO_PROTOCOL_OBTAIN = RTEMS_EVENT_18,
  TQ_EVENT_MUTEX_NO_PROTOCOL_RELEASE = RTEMS_EVENT_19,
  TQ_EVENT_ENQUEUE_FATAL = RTEMS_EVENT_20,
  TQ_EVENT_MUTEX_C_OBTAIN = RTEMS_EVENT_21,
  TQ_EVENT_MUTEX_C_RELEASE = RTEMS_EVENT_22,
  TQ_EVENT_MUTEX_FIFO_OBTAIN = RTEMS_EVENT_23,
  TQ_EVENT_MUTEX_FIFO_RELEASE = RTEMS_EVENT_24,
  TQ_EVENT_ENQUEUE_TIMED = RTEMS_EVENT_25,
  TQ_EVENT_MUTEX_D_OBTAIN = RTEMS_EVENT_26,
  TQ_EVENT_MUTEX_D_RELEASE = RTEMS_EVENT_27,
  TQ_EVENT_PIN = RTEMS_EVENT_28,
  TQ_EVENT_UNPIN = RTEMS_EVENT_29,
  TQ_EVENT_COUNT = RTEMS_EVENT_30
} TQEvent;

typedef enum {
  TQ_ENQUEUE_BLOCKS,
  TQ_ENQUEUE_STICKY
} TQEnqueueVariant;

typedef struct TQContext {
  /**
   * @brief This member defines the thread queue discipline.
   */
  TQDiscipline discipline;

  /**
   * @brief This member defines the enqueue wait behaviour.
   *
   * If TQ_NO_WAIT is used, then no thread queue enqueue shall be performed.
   */
  TQWait wait;

  /**
   * @brief This member defines the enqueue variant.
   */
  TQEnqueueVariant enqueue_variant;

  /**
   * @brief This member defines the deadlock enqueue behaviour.
   */
  TQDeadlock deadlock;

  /**
   * @brief This member contains the runner task identifier.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains a reference to the runner task control block.
   */
  rtems_tcb *runner_tcb;

  /**
   * @brief This member contains the worker task identifiers.
   */
  rtems_id worker_id[ TQ_WORKER_COUNT ];

  /**
   * @brief This member contains references to the worker task control
   *   blocks.
   */
  rtems_tcb *worker_tcb[ TQ_WORKER_COUNT ];

  /**
   * @brief When a worker received an event, the corresponding element shall be
   *   set to true.
   */
  volatile bool event_received[ TQ_WORKER_COUNT ];

  /**
   * @brief If this member is true, then the worker shall busy wait on request.
   */
  volatile bool busy_wait[ TQ_WORKER_COUNT ];

  /**
   * @brief When a worker is done processing its current event set, the
   *   corresponding element shall be set to true.
   */
  volatile bool done[ TQ_WORKER_COUNT ];

  /**
   * @brief This member provides the counter used for the worker counters.
   */
  Atomic_Uint counter;

  /**
   * @brief When a worker returned from TQEnqueue() the counter is incremented
   * and stored in this member.
   */
  uint32_t worker_counter[ TQ_WORKER_COUNT ];

  /**
   * @brief This member contains the last return status of a TQEnqueue() of the
   *   corresponding worker.
   */
  Status_Control status[ TQ_WORKER_COUNT ];

  union {
    /**
     * @brief This member contains the identifier of an object providing the
     *   thread queue under test.
     */
    rtems_id thread_queue_id;

    /**
     * @brief This member contains the reference to object containing the
     *   thread queue under test.
     */
    void *thread_queue_object;
  };

  /**
   * @brief This member contains the identifier of priority inheritance
   *   mutexes.
   */
  rtems_id mutex_id[ TQ_MUTEX_COUNT ];

  /**
   * @brief This member provides the scheduler log.
   */
  T_scheduler_log_40 scheduler_log;

  /**
   * @brief This member provides the get properties handler.
   */
  void ( *get_properties )( struct TQContext *, TQWorkerKind );

  /**
   * @brief This member provides the status convert handler.
   */
  Status_Control ( *convert_status )( Status_Control );

  /**
   * @brief This this member specifies how many threads shall be enqueued.
   */
  uint32_t how_many;

  /**
   * @brief This this member contains the count of the least recently flushed
   *   threads.
   */
  uint32_t flush_count;

  /**
   * @brief This this member provides a context to jump back to before the
   *   enqueue.
   */
  jmp_buf before_enqueue;

  /**
   * @brief This member provides the thread queue enqueue prepare handler.
   */
  void ( *enqueue_prepare )( struct TQContext * );

  /**
   * @brief This member provides the thread queue enqueue handler.
   */
  Status_Control ( *enqueue )( struct TQContext *, TQWait );

  /**
   * @brief This member provides the thread queue enqueue done handler.
   */
  void ( *enqueue_done )( struct TQContext * );

  /**
   * @brief This member provides the thread queue surrender handler.
   */
  Status_Control ( *surrender )( struct TQContext * );

  /**
   * @brief This member provides the thread queue flush handler.
   *
   * The second parameter specifies the count of enqueued threads.  While the
   * third parameter is true, all enqueued threads shall be extracted,
   * otherwise the thread queue shall be partially flushed.  The handler shall
   * return the count of flushed threads.
   */
  uint32_t ( *flush )( struct TQContext *, uint32_t, bool );

  /**
   * @brief This member provides the get owner handler.
   */
  rtems_tcb *( *get_owner )( struct TQContext * );
} TQContext;

void TQSend(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
);

void TQSendAndWaitForExecutionStop(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
);

void TQSendAndWaitForIntendToBlock(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
);

void TQSendAndWaitForExecutionStopOrIntendToBlock(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
);

void TQSendAndSynchronizeRunner(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
);

void TQWaitForEventsReceived( const TQContext *ctx, TQWorkerKind worker );

void TQWaitForIntendToBlock( const TQContext *ctx, TQWorkerKind worker );

void TQWaitForExecutionStop( const TQContext *ctx, TQWorkerKind worker );

void TQClearDone( TQContext *ctx, TQWorkerKind worker );

void TQWaitForDone( const TQContext *ctx, TQWorkerKind worker );

void TQSynchronizeRunner( void );

void TQSynchronizeRunner2( void );

void TQResetCounter( TQContext *ctx );

uint32_t TQGetCounter( const TQContext *ctx );

uint32_t TQGetWorkerCounter( const TQContext *ctx, TQWorkerKind worker );

void TQMutexObtain( const TQContext *ctx, TQMutex mutex );

void TQMutexRelease( const TQContext *ctx, TQMutex mutex );

void TQSetPriority(
  const TQContext *ctx,
  TQWorkerKind     worker,
  Priority         priority
);

Priority TQGetPriority( const TQContext *ctx, TQWorkerKind worker );

void TQSetScheduler(
  const TQContext *ctx,
  TQWorkerKind     worker,
  rtems_id         scheduler_id,
  Priority         priority
);

void TQInitialize( TQContext *ctx );

void TQDestroy( TQContext *ctx );

void TQReset( TQContext *ctx );

void TQSortMutexesByID( TQContext *ctx );

void TQGetProperties( TQContext *ctx, TQWorkerKind enqueued_worker );

Status_Control TQConvertStatus( TQContext *ctx, Status_Control status );

void TQEnqueuePrepare( TQContext *ctx );

Status_Control TQEnqueue( TQContext *ctx, TQWait wait );

Status_Control TQEnqueueFatal( TQContext *ctx );

void TQEnqueueDone( TQContext *ctx );

Status_Control TQSurrender( TQContext *ctx );

void TQFlush( TQContext *ctx, bool flush_all );

rtems_tcb *TQGetOwner( TQContext *ctx );

void TQSchedulerRecordStart( TQContext *ctx );

void TQSchedulerRecordStop( TQContext *ctx );

const T_scheduler_event *TQGetNextAny( TQContext *ctx, size_t *index );

const T_scheduler_event *TQGetNextBlock( TQContext *ctx, size_t *index );

const T_scheduler_event *TQGetNextUnblock( TQContext *ctx, size_t *index );

const T_scheduler_event *TQGetNextUpdatePriority(
  TQContext *ctx,
  size_t    *index
);

const T_scheduler_event *TQGetNextAskForHelp( TQContext *ctx, size_t *index );

void TQDoNothing( TQContext *ctx );

Status_Control TQDoNothingSuccessfully( TQContext *ctx );

Status_Control TQConvertStatusClassic( Status_Control status );

Status_Control TQConvertStatusPOSIX( Status_Control status );

void TQEnqueuePrepareDefault( TQContext *ctx );

void TQEnqueueDoneDefault( TQContext *ctx );

Status_Control TQEnqueueClassicSem( TQContext *ctx, TQWait wait );

Status_Control TQSurrenderClassicSem( TQContext *ctx );

rtems_tcb *TQGetOwnerClassicSem( TQContext *ctx );

typedef enum {
  TQ_SEM_BINARY,
  TQ_SEM_COUNTING
} TQSemVariant;

typedef struct TQSemContext {
  /**
   * @brief This member contains the base thread queue test context.
   */
  TQContext base;

  /**
   * @brief This member defines the semaphore variant.
   */
  TQSemVariant variant;

  /**
   * @brief This member provides the semaphore get count handler.
   */
  uint32_t ( *get_count )( struct TQSemContext * );

  /**
   * @brief This member provides the semaphore set count handler.
   */
  void ( *set_count )( struct TQSemContext *, uint32_t );
} TQSemContext;

Status_Control TQSemSurrender( TQSemContext *ctx );

uint32_t TQSemGetCount( TQSemContext *ctx );

void TQSemSetCount( TQSemContext *ctx, uint32_t count );

Status_Control TQSemSurrenderClassic( TQSemContext *ctx );

uint32_t TQSemGetCountClassic( TQSemContext *ctx );

void TQSemSetCountClassic( TQSemContext *ctx, uint32_t count );

typedef enum {
  TQ_MTX_NO_PROTOCOL,
  TQ_MTX_PRIORITY_INHERIT,
  TQ_MTX_PRIORITY_CEILING,
  TQ_MTX_MRSP
} TQMtxProtocol;

typedef enum {
  TQ_MTX_RECURSIVE_ALLOWED,
  TQ_MTX_RECURSIVE_DEADLOCK,
  TQ_MTX_RECURSIVE_UNAVAILABLE
} TQMtxRecursive;

typedef enum {
  TQ_MTX_NO_OWNER_CHECK,
  TQ_MTX_CHECKS_OWNER
} TQMtxOwnerCheck;

typedef struct TQMtxContext {
  /**
   * @brief This member contains the base thread queue test context.
   */
  TQContext base;

  /**
   * @brief This member defines the locking protocol.
   */
  TQMtxProtocol protocol;

  /**
   * @brief This member defines the recursive seize behaviour.
   */
  TQMtxRecursive recursive;

  /**
   * @brief This member defines the owner check behaviour.
   */
  TQMtxOwnerCheck owner_check;

  /**
   * @brief This member defines the priority ceiling of the mutex.
   *
   * Use PRIO_INVALID to indicate that the mutex does not provide a priority
   * ceiling.
   */
  rtems_task_priority priority_ceiling;
} TQMtxContext;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TX_THREAD_QUEUE_H */
