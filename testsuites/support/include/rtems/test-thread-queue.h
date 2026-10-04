/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the functions to test the
 *   @ref RTEMSScoreThreadQueue.
 *
 * The thread queue test framework validates directives which block on a thread
 * queue, for example a semaphore obtain, a mutex seize, a condition variable
 * wait, or a futex wait.  The runner task controls up to nine worker tasks
 * through the events of TQEvent.  A worker processes its events in a fixed
 * order and sets its done flag at the end.
 *
 * A test case validates a directive with these steps:
 *
 * - Embed a TQContext, TQSemContext, TQMtxContext, or TQCondContext in the
 *   context of the test case.
 *
 * - In the setup, set the discipline, the wait behaviour, and the handlers.
 *   The enqueue handler calls the directive under test and returns its status
 *   built with STATUS_BUILD().  Use TQConvertStatusPOSIX() as the status
 *   converter of a POSIX directive and TQConvertStatusClassic() for a Classic
 *   API directive.
 *
 * - Call TQInitialize() in the setup and TQDestroy() in the teardown.
 *
 * - In a post-condition check, call a reusable test run, for example
 *   ScoreTqReqEnqueueFifo_Run().  The test runs drive the workers through the
 *   handlers and call TQReset() themselves.
 *
 * The reusable test runs are in the tr-tq-enqueue-*.c, tr-tq-flush-*.c,
 * tr-tq-surrender-*.c, tr-tq-timeout-*.c, tr-sem-*.c, tr-mtx-*.c, and
 * tr-cond-wait.c files.  See tc-futex-wait.c for a directive with the POSIX
 * status and tc-sys-lock.c for a mutex.
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

#ifndef _RTEMS_TEST_THREAD_QUEUE_H
#define _RTEMS_TEST_THREAD_QUEUE_H

#include <rtems/test-support.h>

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

/**
 * @brief This enumeration provides the kinds of a priority node in a priority
 *   queue.
 *
 * The timeout test runs of the priority inheritance thread queues use it.
 * See tr-tq-timeout-priority-inherit.c.
 */
typedef enum {
  /**
   * @brief The priority node is the only node in the priority queue.
   */
  TQ_NODE_ONLY,

  /**
   * @brief The priority node is the highest priority node in the priority
   *   queue, and at least one other node is in the queue.
   */
  TQ_NODE_VITAL,

  /**
   * @brief The priority node is not the highest priority node in the priority
   *   queue.
   */
  TQ_NODE_DISPENSABLE
} TQNodeKind;

/**
 * @brief This enumeration provides the wait states of a thread at the time of
 *   a timeout.
 *
 * See tr-tq-timeout-priority-inherit.c.
 */
typedef enum {
  /**
   * @brief The thread is blocked.
   */
  TQ_WAIT_STATE_BLOCKED,

  /**
   * @brief The thread intends to block and is not blocked yet.
   */
  TQ_WAIT_STATE_INTEND_TO_BLOCK,

  /**
   * @brief The thread is ready again, and the wait is over.
   */
  TQ_WAIT_STATE_READY_AGAIN
} TQWaitState;

/**
 * @brief This enumeration provides the workers of the thread queue test
 *   framework.
 *
 * TQInitialize() starts one task for each worker.  TQReset() restores the
 * initial priority of each worker except worker F.
 */
typedef enum {
  /**
   * @brief This worker has the initial priority PRIO_HIGH.
   */
  TQ_BLOCKER_A,

  /**
   * @brief This worker has the initial priority PRIO_VERY_HIGH.
   */
  TQ_BLOCKER_B,

  /**
   * @brief This worker has the initial priority PRIO_ULTRA_HIGH.
   */
  TQ_BLOCKER_C,

  /**
   * @brief This worker has the initial priority PRIO_LOW.
   */
  TQ_BLOCKER_D,

  /**
   * @brief This worker has the initial priority PRIO_LOW.
   */
  TQ_BLOCKER_E,

  /**
   * @brief This worker has the initial priority PRIO_LOW.
   */
  TQ_WORKER_F,

  /**
   * @brief This helper has the initial priority PRIO_LOW.
   *
   * The event TQ_EVENT_HELPER_A_SYNC of another worker synchronizes with it.
   */
  TQ_HELPER_A,

  /**
   * @brief This helper has the initial priority PRIO_LOW.
   *
   * The event TQ_EVENT_HELPER_B_SYNC of another worker synchronizes with it.
   */
  TQ_HELPER_B,

  /**
   * @brief This helper has the initial priority PRIO_LOW.
   */
  TQ_HELPER_C,

  /**
   * @brief This member provides the count of workers.
   */
  TQ_WORKER_COUNT
} TQWorkerKind;

/**
 * @brief This enumeration provides the mutexes of the thread queue test
 *   framework.
 *
 * TQInitialize() creates the mutexes.  The workers obtain and release them
 * through the events of TQEvent.
 */
typedef enum {
  /**
   * @brief This mutex uses the priority inheritance locking protocol.
   */
  TQ_MUTEX_A,

  /**
   * @brief This mutex uses the priority inheritance locking protocol.
   */
  TQ_MUTEX_B,

  /**
   * @brief This mutex uses the priority inheritance locking protocol.
   */
  TQ_MUTEX_C,

  /**
   * @brief This mutex uses the priority inheritance locking protocol.
   */
  TQ_MUTEX_D,

  /**
   * @brief This mutex uses the priority discipline without a locking
   *   protocol.
   */
  TQ_MUTEX_NO_PROTOCOL,

  /**
   * @brief This mutex uses the FIFO discipline.
   */
  TQ_MUTEX_FIFO,

  /**
   * @brief This member provides the count of mutexes.
   */
  TQ_MUTEX_COUNT
} TQMutex;

/**
 * @brief This enumeration provides the disciplines of the thread queue under
 *   test.
 */
typedef enum {
  /**
   * @brief The thread queue uses the FIFO discipline.
   */
  TQ_FIFO,

  /**
   * @brief The thread queue uses the priority discipline.
   */
  TQ_PRIORITY
} TQDiscipline;

/**
 * @brief This enumeration provides the wait behaviours of an enqueue.
 */
typedef enum {
  /**
   * @brief The enqueue does not wait.
   */
  TQ_NO_WAIT,

  /**
   * @brief The enqueue waits forever.
   */
  TQ_WAIT_FOREVER,

  /**
   * @brief The enqueue waits with a timeout.
   */
  TQ_WAIT_TIMED
} TQWait;

/**
 * @brief This enumeration provides the deadlock behaviours of an enqueue.
 */
typedef enum {
  /**
   * @brief The directive returns a deadlock status.
   */
  TQ_DEADLOCK_STATUS,

  /**
   * @brief The directive raises a fatal error.  See TQEnqueueFatal().
   */
  TQ_DEADLOCK_FATAL
} TQDeadlock;

/**
 * @brief This enumeration provides the events which control a worker.
 *
 * A worker processes the events of an event set in this order.  Within an
 * item, the order is as listed.
 *
 * 1. TQ_EVENT_HELPER_A_SYNC and TQ_EVENT_HELPER_B_SYNC
 * 2. TQ_EVENT_SCHEDULER_RECORD_START
 * 3. TQ_EVENT_ENQUEUE_PREPARE, TQ_EVENT_ENQUEUE, TQ_EVENT_ENQUEUE_TIMED, and
 *    TQ_EVENT_ENQUEUE_FATAL
 * 4. TQ_EVENT_TIMEOUT
 * 5. TQ_EVENT_FLUSH_ALL and TQ_EVENT_FLUSH_PARTIAL
 * 6. TQ_EVENT_ENQUEUE_DONE and TQ_EVENT_SURRENDER
 * 7. the obtain and the release of mutex A, B, C, and D, of the mutex without
 *    a locking protocol, and of the FIFO mutex
 * 8. TQ_EVENT_PIN and TQ_EVENT_UNPIN
 * 9. TQ_EVENT_SCHEDULER_RECORD_STOP and TQ_EVENT_RUNNER_SYNC
 * 10. TQ_EVENT_COUNT and TQ_EVENT_BUSY_WAIT
 * 11. TQ_EVENT_RUNNER_SYNC_2
 *
 * At the end, the worker sets its done flag.
 *
 * An event set which combines an enqueue with later events lets the worker
 * block before it reaches them.
 */
typedef enum {
  /**
   * @brief The worker calls TQEnqueuePrepare().
   */
  TQ_EVENT_ENQUEUE_PREPARE = RTEMS_EVENT_0,

  /**
   * @brief The worker calls TQEnqueue() with the wait behaviour of the test
   *   context.  It stores the status and counts.
   */
  TQ_EVENT_ENQUEUE = RTEMS_EVENT_1,

  /**
   * @brief The worker calls TQEnqueueDone().
   */
  TQ_EVENT_ENQUEUE_DONE = RTEMS_EVENT_2,

  /**
   * @brief The worker calls TQSurrender() and expects a success.
   */
  TQ_EVENT_SURRENDER = RTEMS_EVENT_3,

  /**
   * @brief The worker sends this event to the runner.  See
   *   TQSynchronizeRunner().
   */
  TQ_EVENT_RUNNER_SYNC = RTEMS_EVENT_4,

  /**
   * @brief The worker sends this event to the runner after the busy wait.  See
   *   TQSynchronizeRunner2().
   */
  TQ_EVENT_RUNNER_SYNC_2 = RTEMS_EVENT_5,

  /**
   * @brief The worker sends TQ_EVENT_RUNNER_SYNC to helper A.
   */
  TQ_EVENT_HELPER_A_SYNC = RTEMS_EVENT_6,

  /**
   * @brief The worker sends TQ_EVENT_RUNNER_SYNC to helper B.
   */
  TQ_EVENT_HELPER_B_SYNC = RTEMS_EVENT_7,

  /**
   * @brief The worker obtains mutex A.
   */
  TQ_EVENT_MUTEX_A_OBTAIN = RTEMS_EVENT_8,

  /**
   * @brief The worker releases mutex A.
   */
  TQ_EVENT_MUTEX_A_RELEASE = RTEMS_EVENT_9,

  /**
   * @brief The worker obtains mutex B.
   */
  TQ_EVENT_MUTEX_B_OBTAIN = RTEMS_EVENT_10,

  /**
   * @brief The worker releases mutex B.
   */
  TQ_EVENT_MUTEX_B_RELEASE = RTEMS_EVENT_11,

  /**
   * @brief The worker busy waits while its busy wait flag of the test context
   *   is set.
   */
  TQ_EVENT_BUSY_WAIT = RTEMS_EVENT_12,

  /**
   * @brief The worker calls TQFlush() to extract all enqueued threads.
   */
  TQ_EVENT_FLUSH_ALL = RTEMS_EVENT_13,

  /**
   * @brief The worker calls TQFlush() for a partial flush.
   */
  TQ_EVENT_FLUSH_PARTIAL = RTEMS_EVENT_14,

  /**
   * @brief The worker calls TQSchedulerRecordStart().
   */
  TQ_EVENT_SCHEDULER_RECORD_START = RTEMS_EVENT_15,

  /**
   * @brief The worker calls TQSchedulerRecordStop().
   */
  TQ_EVENT_SCHEDULER_RECORD_STOP = RTEMS_EVENT_16,

  /**
   * @brief The worker runs the timeout of its thread with TQTimeout() while
   *   thread dispatching is disabled.
   */
  TQ_EVENT_TIMEOUT = RTEMS_EVENT_17,

  /**
   * @brief The worker obtains the mutex without a locking protocol.
   */
  TQ_EVENT_MUTEX_NO_PROTOCOL_OBTAIN = RTEMS_EVENT_18,

  /**
   * @brief The worker releases the mutex without a locking protocol.
   */
  TQ_EVENT_MUTEX_NO_PROTOCOL_RELEASE = RTEMS_EVENT_19,

  /**
   * @brief The worker enqueues and catches a deadlock fatal error.  The stored
   *   status is STATUS_DEADLOCK after the fatal error.
   */
  TQ_EVENT_ENQUEUE_FATAL = RTEMS_EVENT_20,

  /**
   * @brief The worker obtains mutex C.
   */
  TQ_EVENT_MUTEX_C_OBTAIN = RTEMS_EVENT_21,

  /**
   * @brief The worker releases mutex C.
   */
  TQ_EVENT_MUTEX_C_RELEASE = RTEMS_EVENT_22,

  /**
   * @brief The worker obtains the mutex with the FIFO discipline.
   */
  TQ_EVENT_MUTEX_FIFO_OBTAIN = RTEMS_EVENT_23,

  /**
   * @brief The worker releases the mutex with the FIFO discipline.
   */
  TQ_EVENT_MUTEX_FIFO_RELEASE = RTEMS_EVENT_24,

  /**
   * @brief The worker calls TQEnqueue() with TQ_WAIT_TIMED.  It stores the
   *   status and counts.
   */
  TQ_EVENT_ENQUEUE_TIMED = RTEMS_EVENT_25,

  /**
   * @brief The worker obtains mutex D.
   */
  TQ_EVENT_MUTEX_D_OBTAIN = RTEMS_EVENT_26,

  /**
   * @brief The worker releases mutex D.
   */
  TQ_EVENT_MUTEX_D_RELEASE = RTEMS_EVENT_27,

  /**
   * @brief The worker pins its thread to its processor.
   */
  TQ_EVENT_PIN = RTEMS_EVENT_28,

  /**
   * @brief The worker unpins its thread.
   */
  TQ_EVENT_UNPIN = RTEMS_EVENT_29,

  /**
   * @brief The worker increments the counter of the test context and stores
   *   the value as its worker counter.
   */
  TQ_EVENT_COUNT = RTEMS_EVENT_30
} TQEvent;

/**
 * @brief This enumeration provides the variants of an enqueue.
 */
typedef enum {
  /**
   * @brief The enqueue blocks the thread.
   */
  TQ_ENQUEUE_BLOCKS,

  /**
   * @brief The enqueue keeps the thread in its scheduler node and busy waits.
   *
   * The MrsP semaphores use this variant.
   */
  TQ_ENQUEUE_STICKY
} TQEnqueueVariant;

/**
 * @brief This structure provides the thread queue test context.
 *
 * A test case embeds it and sets the handlers and the behaviours before
 * TQInitialize().  See the overview of this file.
 */
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

  /**
   * @brief This union identifies the object which contains the thread queue
   *   under test.
   */
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

/**
 * @brief Sends the events to the worker.
 *
 * In SMP configurations, the function clears the event received flag of the
 * worker first.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 * @param events is the set of events to send.
 */
void TQSend( TQContext *ctx, TQWorkerKind worker, rtems_event_set events );

/**
 * @brief Sends the events to the worker and waits until the worker executes on
 *   no processor.
 *
 * In SMP configurations, the function also waits until the worker received the
 * events.  In uniprocessor configurations, it only sends the events.
 *
 * This is the usual way to let a blocker enqueue on the thread queue under
 * test.  Send TQ_EVENT_ENQUEUE and wait.  In SMP configurations, the worker
 * runs on another processor, so the wait lets the block complete before the
 * next step.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 * @param events is the set of events to send.
 */
void TQSendAndWaitForExecutionStop(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
);

/**
 * @brief Sends the events to the worker and waits until the worker intends to
 *   block on a thread queue.
 *
 * In SMP configurations, the function also waits until the worker received the
 * events.  In uniprocessor configurations, it only sends the events.
 *
 * Use this function when the next step shall act while the worker is inside
 * the enqueue and has not blocked yet.  See tr-tq-surrender-mrsp.c.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 * @param events is the set of events to send.
 */
void TQSendAndWaitForIntendToBlock(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
);

/**
 * @brief Sends the events to the worker and waits until the worker executes on
 *   no processor or intends to block on a thread queue.
 *
 * In SMP configurations, the function also waits until the worker received the
 * events.  In uniprocessor configurations, it only sends the events.
 *
 * Use this function when the enqueue may block or return, for example a mutex
 * seize which may succeed.  Call TQClearDone() before and TQWaitForDone()
 * afterwards.  See tr-mtx-seize-wait.c.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 * @param events is the set of events to send.
 */
void TQSendAndWaitForExecutionStopOrIntendToBlock(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
);

/**
 * @brief Sends the events and the runner synchronization event to the worker
 *   and waits for the synchronization.
 *
 * The test records a failure, if the runner synchronization event is pending
 * before the send.
 *
 * Use this function when the runner shall continue after the worker processed
 * the events, for example after a surrender by the worker.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 * @param events is the set of events to send.
 */
void TQSendAndSynchronizeRunner(
  TQContext      *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
);

/**
 * @brief Waits until the worker received its events.
 *
 * The wait has no bound.  In uniprocessor configurations, the function returns
 * at once.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 */
void TQWaitForEventsReceived( const TQContext *ctx, TQWorkerKind worker );

/**
 * @brief Waits until the worker intends to block on a thread queue.
 *
 * The wait has no bound.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 */
void TQWaitForIntendToBlock( const TQContext *ctx, TQWorkerKind worker );

/**
 * @brief Waits until the worker executes on no processor.
 *
 * The wait has no bound.  In uniprocessor configurations, the function returns
 * at once.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 */
void TQWaitForExecutionStop( const TQContext *ctx, TQWorkerKind worker );

/**
 * @brief Clears the done flag of the worker.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 */
void TQClearDone( TQContext *ctx, TQWorkerKind worker );

/**
 * @brief Waits until the worker processed its event set.
 *
 * The worker sets its done flag at the end of each event set.  The wait has no
 * bound.
 *
 * Call TQClearDone() before the event send and this function after it.  See
 * tr-mtx-seize-wait.c.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 */
void TQWaitForDone( const TQContext *ctx, TQWorkerKind worker );

/**
 * @brief Waits for the runner synchronization event.
 *
 * A worker sends the event for TQ_EVENT_RUNNER_SYNC.
 */
void TQSynchronizeRunner( void );

/**
 * @brief Waits for both runner synchronization events.
 *
 * A worker sends the events for TQ_EVENT_RUNNER_SYNC and
 * TQ_EVENT_RUNNER_SYNC_2.
 */
void TQSynchronizeRunner2( void );

/**
 * @brief Resets the counter and the worker counters.
 *
 * @param ctx is the thread queue test context.
 */
void TQResetCounter( TQContext *ctx );

/**
 * @brief Gets the counter.
 *
 * @param ctx is the thread queue test context.
 *
 * @return Returns the value of the counter.
 */
uint32_t TQGetCounter( const TQContext *ctx );

/**
 * @brief Gets the counter value of the worker.
 *
 * A worker stores the incremented counter after each return from TQEnqueue()
 * and for TQ_EVENT_COUNT.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 *
 * @return Returns the counter value of the worker.
 */
uint32_t TQGetWorkerCounter( const TQContext *ctx, TQWorkerKind worker );

/**
 * @brief Obtains the mutex of the test context.
 *
 * The function waits forever.  The test records a failure, if
 * rtems_semaphore_obtain() does not succeed.
 *
 * @param ctx is the thread queue test context.
 * @param mutex is the mutex.
 */
void TQMutexObtain( const TQContext *ctx, TQMutex mutex );

/**
 * @brief Releases the mutex of the test context.
 *
 * The test records a failure, if rtems_semaphore_release() does not succeed.
 *
 * @param ctx is the thread queue test context.
 * @param mutex is the mutex.
 */
void TQMutexRelease( const TQContext *ctx, TQMutex mutex );

/**
 * @brief Sets the priority of the worker.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 * @param priority is the new priority.
 */
void TQSetPriority(
  const TQContext *ctx,
  TQWorkerKind     worker,
  Priority         priority
);

/**
 * @brief Gets the priority of the worker.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 *
 * @return Returns the current priority of the worker.
 */
Priority TQGetPriority( const TQContext *ctx, TQWorkerKind worker );

/**
 * @brief Sets the home scheduler and the priority of the worker.
 *
 * In uniprocessor configurations, the function sets only the priority.
 *
 * @param ctx is the thread queue test context.
 * @param worker is the worker.
 * @param scheduler_id is the identifier of the new home scheduler.
 * @param priority is the priority with respect to the new home scheduler.
 */
void TQSetScheduler(
  const TQContext *ctx,
  TQWorkerKind     worker,
  rtems_id         scheduler_id,
  Priority         priority
);

/**
 * @brief Initializes the thread queue test context.
 *
 * The executing task becomes the runner.  The function creates the mutexes and
 * starts the workers.  Afterwards, the runner has the priority PRIO_NORMAL.
 *
 * @param ctx is the thread queue test context.
 */
void TQInitialize( TQContext *ctx );

/**
 * @brief Destroys the thread queue test context.
 *
 * The function deletes the workers and the mutexes and restores the priority
 * of the runner.
 *
 * @param ctx is the thread queue test context.
 */
void TQDestroy( TQContext *ctx );

/**
 * @brief Resets the home schedulers and the priorities of the runner and the
 *   workers.
 *
 * The function moves the runner and all workers except worker F to the
 * scheduler with the identifier SCHEDULER_A_ID.  Each task gets its initial
 * priority.
 *
 * @param ctx is the thread queue test context.
 */
void TQReset( TQContext *ctx );

/**
 * @brief Sorts the mutexes A, B, and C of the test context by identifier.
 *
 * Afterwards, the identifiers of the three mutexes ascend.
 *
 * The deadlock tests call this function first, so that the lock order of the
 * mutexes follows their identifiers.  See tr-tq-enqueue-deadlock.c.
 *
 * @param ctx is the thread queue test context.
 */
void TQSortMutexesByID( TQContext *ctx );

/**
 * @brief Calls the get properties handler of the test context.
 *
 * @param ctx is the thread queue test context.
 * @param enqueued_worker is the worker which enqueues on the thread queue.
 */
void TQGetProperties( TQContext *ctx, TQWorkerKind enqueued_worker );

/**
 * @brief Converts the status with the status convert handler of the test
 *   context.
 *
 * @param ctx is the thread queue test context.
 * @param status is the status to convert.
 *
 * @return Returns the converted status.
 */
Status_Control TQConvertStatus( TQContext *ctx, Status_Control status );

/**
 * @brief Calls the enqueue prepare handler of the test context.
 *
 * @param ctx is the thread queue test context.
 */
void TQEnqueuePrepare( TQContext *ctx );

/**
 * @brief Calls the enqueue handler of the test context.
 *
 * @param ctx is the thread queue test context.
 * @param wait is the wait behaviour of the enqueue.
 *
 * @return Returns the status of the enqueue.
 */
Status_Control TQEnqueue( TQContext *ctx, TQWait wait );

/**
 * @brief Enqueues on the thread queue and expects a deadlock fatal error.
 *
 * A fatal handler catches the thread queue deadlock error and returns to this
 * function.
 *
 * The deadlock tests use this function for thread queues which report a
 * deadlock with a fatal error.  See tr-tq-enqueue-deadlock.c.
 *
 * @param ctx is the thread queue test context.
 *
 * @return Returns STATUS_DEADLOCK after a deadlock fatal error, otherwise the
 *   status of the enqueue.
 */
Status_Control TQEnqueueFatal( TQContext *ctx );

/**
 * @brief Calls the enqueue done handler of the test context.
 *
 * @param ctx is the thread queue test context.
 */
void TQEnqueueDone( TQContext *ctx );

/**
 * @brief Calls the surrender handler of the test context.
 *
 * @param ctx is the thread queue test context.
 *
 * @return Returns the status of the surrender.
 */
Status_Control TQSurrender( TQContext *ctx );

/**
 * @brief Flushes the thread queue with the flush handler of the test context.
 *
 * The function stores the count of flushed threads in the test context.
 *
 * @param ctx is the thread queue test context.
 * @param flush_all is true to extract all enqueued threads, otherwise the
 *   flush is partial.
 */
void TQFlush( TQContext *ctx, bool flush_all );

/**
 * @brief Gets the owner of the thread queue with the get owner handler of the
 *   test context.
 *
 * @param ctx is the thread queue test context.
 *
 * @return Returns the owner, or NULL if the test context has no get owner
 *   handler.
 */
rtems_tcb *TQGetOwner( TQContext *ctx );

/**
 * @brief Starts to record the scheduler operations in the scheduler log of the
 *   test context.
 *
 * The test records a failure, if another scheduler log records already.
 *
 * Send TQ_EVENT_SCHEDULER_RECORD_START and TQ_EVENT_SCHEDULER_RECORD_STOP to
 * the worker around the action under test.  Then iterate the log with
 * TQGetNextUnblock() or another iterator and compare the threads with the
 * expected order.  See tr-tq-flush-fifo.c.
 *
 * @param ctx is the thread queue test context.
 */
void TQSchedulerRecordStart( TQContext *ctx );

/**
 * @brief Stops to record the scheduler operations.
 *
 * The test records a failure, if the scheduler log of the test context did not
 * record.
 *
 * @param ctx is the thread queue test context.
 */
void TQSchedulerRecordStop( TQContext *ctx );

/**
 * @brief Gets the next scheduler event of the scheduler log.
 *
 * @param ctx is the thread queue test context.
 * @param index is the index of the next event, which the function updates.
 *
 * @return Returns the next event, or the null event at the end of the log.
 */
const T_scheduler_event *TQGetNextAny( TQContext *ctx, size_t *index );

/**
 * @brief Gets the next block event of the scheduler log.
 *
 * @param ctx is the thread queue test context.
 * @param index is the index of the next event, which the function updates.
 *
 * @return Returns the next block event, or the null event at the end of the
 *   log.
 */
const T_scheduler_event *TQGetNextBlock( TQContext *ctx, size_t *index );

/**
 * @brief Gets the next unblock event of the scheduler log.
 *
 * Start with an index of zero.  See TQSchedulerRecordStart().
 *
 * @param ctx is the thread queue test context.
 * @param index is the index of the next event, which the function updates.
 *
 * @return Returns the next unblock event, or the null event at the end of the
 *   log.
 */
const T_scheduler_event *TQGetNextUnblock( TQContext *ctx, size_t *index );

/**
 * @brief Gets the next update priority event of the scheduler log.
 *
 * @param ctx is the thread queue test context.
 * @param index is the index of the next event, which the function updates.
 *
 * @return Returns the next update priority event, or the null event at the end
 *   of the log.
 */
const T_scheduler_event *TQGetNextUpdatePriority(
  TQContext *ctx,
  size_t    *index
);

/**
 * @brief Gets the next ask for help event of the scheduler log.
 *
 * @param ctx is the thread queue test context.
 * @param index is the index of the next event, which the function updates.
 *
 * @return Returns the next ask for help event, or the null event at the end of
 *   the log.
 */
const T_scheduler_event *TQGetNextAskForHelp( TQContext *ctx, size_t *index );

/**
 * @brief Does nothing.
 *
 * A test context uses this function as an empty handler.
 *
 * @param ctx is the thread queue test context.
 */
void TQDoNothing( TQContext *ctx );

/**
 * @brief Does nothing and returns STATUS_SUCCESSFUL.
 *
 * A test context uses this function as a handler without an effect.
 *
 * @param ctx is the thread queue test context.
 *
 * @return Returns STATUS_SUCCESSFUL.
 */
Status_Control TQDoNothingSuccessfully( TQContext *ctx );

/**
 * @brief Keeps the Classic API part of the status.
 *
 * @param status is the status to convert.
 *
 * @return Returns the status with the Classic API status code alone.
 */
Status_Control TQConvertStatusClassic( Status_Control status );

/**
 * @brief Keeps the POSIX part of the status.
 *
 * @param status is the status to convert.
 *
 * @return Returns the status with the POSIX error number alone.
 */
Status_Control TQConvertStatusPOSIX( Status_Control status );

/**
 * @brief Enqueues on the thread queue without a wait and expects a success.
 *
 * A test context uses this function as its enqueue prepare handler.
 *
 * @param ctx is the thread queue test context.
 */
void TQEnqueuePrepareDefault( TQContext *ctx );

/**
 * @brief Surrenders the thread queue and expects a success.
 *
 * A test context uses this function as its enqueue done handler.
 *
 * @param ctx is the thread queue test context.
 */
void TQEnqueueDoneDefault( TQContext *ctx );

/**
 * @brief Obtains the Classic API semaphore of the test context.
 *
 * TQ_WAIT_FOREVER waits forever, and TQ_WAIT_TIMED waits with the longest
 * timeout.  TQ_NO_WAIT does not wait.
 *
 * @param ctx is the thread queue test context.
 * @param wait is the wait behaviour of the obtain.
 *
 * @return Returns the status of rtems_semaphore_obtain().
 */
Status_Control TQEnqueueClassicSem( TQContext *ctx, TQWait wait );

/**
 * @brief Releases the Classic API semaphore of the test context.
 *
 * @param ctx is the thread queue test context.
 *
 * @return Returns the status of rtems_semaphore_release().
 */
Status_Control TQSurrenderClassicSem( TQContext *ctx );

/**
 * @brief Gets the owner of the Classic API semaphore of the test context.
 *
 * An invalid semaphore identifier ends the test case.
 *
 * @param ctx is the thread queue test context.
 *
 * @return Returns the owner, or NULL if no thread owns the semaphore.
 */
rtems_tcb *TQGetOwnerClassicSem( TQContext *ctx );

/**
 * @brief This enumeration provides the variants of a semaphore under test.
 */
typedef enum {
  /**
   * @brief The semaphore is a binary semaphore.
   */
  TQ_SEM_BINARY,

  /**
   * @brief The semaphore is a counting semaphore.
   */
  TQ_SEM_COUNTING
} TQSemVariant;

/**
 * @brief This structure provides the test context of a semaphore.
 */
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

/**
 * @brief Gets the count of the semaphore with the get count handler of the
 *   test context.
 *
 * @param ctx is the semaphore test context.
 *
 * @return Returns the count of the semaphore.
 */
uint32_t TQSemGetCount( TQSemContext *ctx );

/**
 * @brief Sets the count of the semaphore with the set count handler of the
 *   test context.
 *
 * @param ctx is the semaphore test context.
 * @param count is the new count.
 */
void TQSemSetCount( TQSemContext *ctx, uint32_t count );

/**
 * @brief Gets the count of the Classic API semaphore of the test context.
 *
 * An invalid semaphore identifier ends the test case.
 *
 * @param ctx is the semaphore test context.
 *
 * @return Returns the count of the semaphore.
 */
uint32_t TQSemGetCountClassic( TQSemContext *ctx );

/**
 * @brief Sets the count of the Classic API semaphore of the test context.
 *
 * The function writes the count directly to the semaphore object.  An invalid
 * semaphore identifier ends the test case.
 *
 * @param ctx is the semaphore test context.
 * @param count is the new count.
 */
void TQSemSetCountClassic( TQSemContext *ctx, uint32_t count );

/**
 * @brief This enumeration provides the locking protocols of a mutex under
 *   test.
 */
typedef enum {
  /**
   * @brief The mutex uses no locking protocol.
   */
  TQ_MTX_NO_PROTOCOL,

  /**
   * @brief The mutex uses the priority inheritance locking protocol.
   */
  TQ_MTX_PRIORITY_INHERIT,

  /**
   * @brief The mutex uses the priority ceiling locking protocol.
   */
  TQ_MTX_PRIORITY_CEILING,

  /**
   * @brief The mutex uses the MrsP locking protocol.
   */
  TQ_MTX_MRSP
} TQMtxProtocol;

/**
 * @brief This enumeration provides the behaviours of a recursive seize of a
 *   mutex under test.
 */
typedef enum {
  /**
   * @brief A recursive seize succeeds.
   */
  TQ_MTX_RECURSIVE_ALLOWED,

  /**
   * @brief A recursive seize is a deadlock.
   */
  TQ_MTX_RECURSIVE_DEADLOCK,

  /**
   * @brief A recursive seize returns the unavailable status.
   */
  TQ_MTX_RECURSIVE_UNAVAILABLE
} TQMtxRecursive;

/**
 * @brief This enumeration provides the owner check behaviours of a surrender
 *   of a mutex under test.
 */
typedef enum {
  /**
   * @brief The surrender does not check the owner.
   */
  TQ_MTX_NO_OWNER_CHECK,

  /**
   * @brief The surrender checks that the calling thread owns the mutex.
   */
  TQ_MTX_CHECKS_OWNER
} TQMtxOwnerCheck;

/**
 * @brief This structure provides the test context of a mutex.
 */
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

/**
 * @brief This structure provides the test context of a condition variable.
 */
typedef struct TQCondContext {
  /**
   * @brief This member contains the base thread queue test context.
   */
  TQContext base;

  /**
   * @brief This member contains the test context of the mutex under the
   *   condition variable.
   */
  TQMtxContext mtx;

  /**
   * @brief This member defines how often the calling thread seizes the mutex
   *   before it waits on the condition variable.
   */
  unsigned int nest;

  /**
   * @brief This member contains the owner of the mutex after the wait on the
   *   condition variable and before the release of the mutex.
   */
  rtems_tcb *owner_after;

  /**
   * @brief This member contains the nest level of the mutex after the wait on
   *   the condition variable and before the release of the mutex.
   */
  unsigned int nest_after;
} TQCondContext;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_TEST_THREAD_QUEUE_H */
