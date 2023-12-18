/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the support functions for the validation
 *   test cases.
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

#ifndef _TX_SUPPORT_H
#define _TX_SUPPORT_H

#include <rtems.h>
#include <rtems/irq-extension.h>
#include <rtems/score/atomic.h>
#include <rtems/score/threadq.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSTestSuitesValidation
 *
 * @{
 */

typedef enum {
  PRIO_PSEUDO_ISR,
  PRIO_VERY_ULTRA_HIGH,
  PRIO_ULTRA_HIGH,
  PRIO_VERY_HIGH,
  PRIO_HIGH,
  PRIO_NORMAL,
  PRIO_LOW,
  PRIO_VERY_LOW,
  PRIO_ULTRA_LOW
} Priority;

/**
 * @brief This constants represents the default priority of the runner task.
 */
#define PRIO_DEFAULT 1

/**
 * @brief This constants represents an invalid RTEMS task priority value.
 *
 * It should be an invalid priority value which is not equal to
 * RTEMS_CURRENT_PRIORITY and RTEMS_TIMER_SERVER_DEFAULT_PRIORITY.
 */
#define PRIO_INVALID 0xfffffffe

/**
 * @brief This constants represents a priority which is close to the priority
 *   of the idle thread.
 *
 * It may be used for the runner thread together with PRIO_FLEXIBLE for worker
 * threads.
 */
#define PRIO_NEARLY_IDLE 126

/**
 * @brief This constants represents a priority with a wider range of higher and
 *   lower priorities around it.
 *
 * It may be used for the worker threads together with PRIO_NEARLY_IDLE for the
 * runner thread.
 */
#define PRIO_FLEXIBLE 64

/**
 * @brief This constants represents an invalid RTEMS object identifier.
 */
#define INVALID_ID 0xfffffffd

/**
 * @brief This constants represents an object name for tests.
 */
#define OBJECT_NAME rtems_build_name( 'T', 'E', 'S', 'T' )

#define CreateTask( name, priority ) \
  DoCreateTask( \
    rtems_build_name( name[ 0 ], name[ 1 ], name[ 2 ], name[ 3 ] ), \
    priority \
  )

#define SCHEDULER_A_ID 0xf010001

#define SCHEDULER_B_ID 0xf010002

#define SCHEDULER_C_ID 0xf010003

#define SCHEDULER_D_ID 0xf010004

rtems_id DoCreateTask( rtems_name name, rtems_task_priority priority );

void StartTask( rtems_id id, rtems_task_entry entry, void *arg );

void DeleteTask( rtems_id id );

void SuspendTask( rtems_id id );

void SuspendSelf( void );

void ResumeTask( rtems_id id );

bool IsTaskSuspended( rtems_id id );

rtems_event_set QueryPendingEvents( void );

rtems_event_set PollAnyEvents( void );

rtems_event_set ReceiveAnyEvents( void );

rtems_event_set ReceiveAnyEventsTimed( rtems_interval ticks );

void ReceiveAllEvents( rtems_event_set events );

void SendEvents( rtems_id id, rtems_event_set events );

rtems_mode GetMode( void );

rtems_mode SetMode( rtems_mode set, rtems_mode mask );

rtems_task_priority GetPriority( rtems_id id );

rtems_task_priority GetPriorityByScheduler(
  rtems_id task_id,
  rtems_id scheduler_id
);

rtems_task_priority SetPriority( rtems_id id, rtems_task_priority priority );

rtems_task_priority GetSelfPriority( void );

rtems_task_priority SetSelfPriority( rtems_task_priority priority );

rtems_task_priority SetSelfPriorityNoYield( rtems_task_priority priority );

rtems_id GetScheduler( rtems_id id );

rtems_id GetSelfScheduler( void );

void SetScheduler(
  rtems_id            task_id,
  rtems_id            scheduler_id,
  rtems_task_priority priority
);

void SetSelfScheduler( rtems_id scheduler_id, rtems_task_priority priority );

void GetAffinity( rtems_id id, cpu_set_t *set );

void GetSelfAffinity( cpu_set_t *set );

void SetAffinity( rtems_id id, const cpu_set_t *set );

void SetSelfAffinity( const cpu_set_t *set );

void SetAffinityOne( rtems_id id, uint32_t cpu_index );

void SetSelfAffinityOne( uint32_t cpu_index );

void SetAffinityAll( rtems_id id );

void SetSelfAffinityAll( void );

void Yield( void );

void YieldTask( rtems_id id );

void AddProcessor( rtems_id scheduler_id, uint32_t cpu_index );

void RemoveProcessor( rtems_id scheduler_id, uint32_t cpu_index );

rtems_id CreateMutex( void );

rtems_id CreateMutexNoProtocol( void );

rtems_id CreateMutexFIFO( void );

bool IsMutexOwner( rtems_id id );

void DeleteMutex( rtems_id id );

void ObtainMutex( rtems_id id );

void ObtainMutexTimed( rtems_id id, rtems_interval ticks );

void ObtainMutexDeadlock( rtems_id id );

void ReleaseMutex( rtems_id id );

struct Thread_queue_Queue;

struct Thread_queue_Queue *GetMutexThreadQueue( rtems_id id );

void RestoreRunnerASR( void );

void RestoreRunnerMode( void );

void RestoreRunnerPriority( void );

void RestoreRunnerScheduler( void );

struct _Thread_Control;

struct _Thread_Control *GetThread( rtems_id id );

struct _Thread_Control *GetExecuting( void );

void KillZombies( void );

void WaitForExecutionStop( rtems_id task_id );

void WaitForIntendToBlock( rtems_id task_id );

void WaitForHeir( uint32_t cpu_index, rtems_id task_id );

void WaitForNextTask( uint32_t cpu_index, rtems_id task_id );

typedef enum {
  TASK_TIMER_INVALID,
  TASK_TIMER_INACTIVE,
  TASK_TIMER_TICKS,
  TASK_TIMER_REALTIME,
  TASK_TIMER_MONOTONIC
} TaskTimerState;

typedef struct {
  TaskTimerState state;
  uint64_t expire_ticks;
  struct timespec expire_timespec;
} TaskTimerInfo;

void GetTaskTimerInfo( rtems_id id, TaskTimerInfo *info );

void GetTaskTimerInfoByThread(
  struct _Thread_Control *thread,
  TaskTimerInfo          *info
);

void ClockTick( void );

/**
 * @brief Simulates a clock tick with the final expire time point of
 *   UINT64_MAX for all clocks.
 *
 * This function does not update the clock ticks counter.
 */
void FinalClockTick( void );

/**
 * @brief Simulates a single clock tick using the software timecounter.
 *
 * In contrast to ClockTick(), this function updates also CLOCK_MONOTONIC and
 * CLOCK_REALTIME to the next software timecounter clock tick time point.
 *
 * This function is designed for test suites not having a clock driver.
 */
void TimecounterTick( void );

typedef uint32_t ( *GetTimecountHandler )( void );

/**
 * @brief Sets the get timecount handler.
 *
 * Using this function will replace the timecounter of the clock driver.
 *
 * @return Returns the previous get timecount handler.
 */
GetTimecountHandler SetGetTimecountHandler( GetTimecountHandler handler );

/**
 * @brief This constant represents the fake frequency of the software
 *   timecounter.
 */
#define SOFTWARE_TIMECOUNTER_FREQUENCY 1000000

/**
 * @brief Gets the software timecount counter value.
 *
 * @return Returns the current software timecounter counter value.
 */
uint32_t GetTimecountCounter( void );

/**
 * @brief Sets and gets the software timecount counter value.
 *
 * @param counter is the new software timecounter counter value.
 *
 * @return Returns the previous software timecounter counter value.
 */
uint32_t SetTimecountCounter( uint32_t counter );

/**
 * @brief Return the task id of the timer server task
 *
 * This function is an attempt to avoid using RTEMS internal global
 * _Timer_server throughout the validation test code.
 *
 * @return Returns the task id of the timer server task, if
 *   rtems_timer_initiate_server() has been invoked before,
 *   otherwise - if the timer server task does not exist -
 *   RTEMS_INVALID_ID is returned.
 */
rtems_id GetTimerServerTaskId( void );

/**
 * @brief Undo the effects of rtems_timer_initiate_server()
 *
 * If rtems_timer_initiate_server() was never called before,
 * nothing is done.
 *
 * If rtems_timer_initiate_server() was called before, the
 * created thread and other resources are freed so that
 * rtems_timer_initiate_server() can be called again.
 * There should be no pending timers which are not yet executed
 * by the server task. Naturally, there should be no
 * timer server timers scheduled for execution.
 *
 * @return Returns true, if rtems_timer_initiate_server() has been
 *   invoked before and the timer server task has indeed been deleted,
 *   otherwise false.
 */
bool DeleteTimerServer( void );

typedef struct {
  struct {
    const void *begin;
    void *free_begin;
    const void *end;
  } areas[ 2 ];
  size_t count;
} MemoryContext;

void MemorySave( MemoryContext *ctx );

void MemoryRestore( const MemoryContext *ctx );

/**
 * @brief Fails a dynamic memory allocation when the counter reaches zero.
 *
 * This function initializes an internal counter which is decremented before
 * each dynamic memory allocation though the rtems_malloc() directive.  When
 * the counter decrements from one to zero, the allocation fails and NULL will
 * be returned.
 *
 * @param counter is the initial counter value.
 */
void MemoryAllocationFailWhen( uint32_t counter );

typedef struct {
  Chain_Node node;
  void ( *handler )( void * );
  void *arg;
  Atomic_Uint done;
} CallWithinISRRequest;

void CallWithinISR( void ( *handler )( void * ), void *arg );

void CallWithinISRSubmit( CallWithinISRRequest *request );

void CallWithinISRWait( const CallWithinISRRequest *request );

void CallWithinISRRaise( void );

void CallWithinISRClear( void );

rtems_vector_number CallWithinISRGetVector( void );

rtems_vector_number GetSoftwareInterruptVector( void );

typedef struct {
  Thread_queue_Operations        tq_ops;
  const Thread_queue_Operations *wrapped_ops;
  Thread_queue_Control           thread_queue;
  CallWithinISRRequest           isr_request;
} WrapThreadQueueContext;

void WrapThreadQueueInitialize(
  WrapThreadQueueContext *ctx,
  void                 ( *handler )( void * ),
  void                   *arg
);

void WrapThreadQueueExtract(
  WrapThreadQueueContext *ctx,
  struct _Thread_Control *thread
);

void WrapThreadQueueExtractDirect(
  WrapThreadQueueContext *ctx,
  Thread_Control         *thread
);

void WrapThreadQueueDestroy( WrapThreadQueueContext *ctx );

struct Per_CPU_Control;

void SetPreemptionIntervention(
  struct Per_CPU_Control *cpu,
  void                 ( *handler )( void * ),
  void                   *arg
);

rtems_vector_number GetValidInterruptVectorNumber(
  const rtems_interrupt_attributes *required
);

rtems_vector_number GetTestableInterruptVector(
  const rtems_interrupt_attributes *required
);

rtems_status_code RaiseSoftwareInterrupt( rtems_vector_number vector );

rtems_status_code ClearSoftwareInterrupt( rtems_vector_number vector );

bool HasInterruptVectorEntriesInstalled( rtems_vector_number vector );

/**
 * @brief Get the clock and context of a timer from RTEMS internal data.
 *
 * With exception of TIMER_DORMANT, the return values are bits or-ed together.
 *
 * @param id The timer ID.
 *
 * @retval TIMER_DORMANT Either the id argument is invalid or the timer has
 *   never been used before.
 * @return The TIMER_CLASS_BIT_ON_TASK is set, if the timer server routine
 *   was or will be executed in task context, otherwise it was or will be
 *   executed in interrupt context.
 *
 *   The TIMER_CLASS_BIT_TIME_OF_DAY is set, if the clock used is or was the
 *   ${/glossary/clock-realtime:/term}, otherwise the
 *   ${/glossary/clock-tick:/term} based clock is or was used.
 */
Timer_Classes GetTimerClass( rtems_id id );

/**
 * @brief This structure provides data used by RTEMS to schedule a timer
 *   service routine.
 */
typedef struct {
  /**
    * @brief This member contains a reference to the timer service routine.
    */
  rtems_timer_service_routine_entry routine;
  /**
   * @brief This member contains a reference to the user data to be provided
   * to the timer service routine.
   */
  void *user_data;
  /**
   * @brief This member contains the timer interval in ticks or seconds.
   */
  Watchdog_Interval interval;
} Timer_Scheduling_Data;

/**
 * @brief Get data related to scheduling a timer service routine
 *   from RTEMS internal structures.
 *
 * @param id The timer ID.
 * @param[out] data If the reference is not NULL, the data retrieved from
 *   internal RTEMS structures is stored here.
 */
void GetTimerSchedulingData(
  rtems_id id,
  Timer_Scheduling_Data *data
);

/**
 * @brief The various states of a timer.
 */
typedef enum {
  TIMER_INVALID,
  TIMER_INACTIVE,
  TIMER_SCHEDULED,
  TIMER_PENDING
} Timer_States;

/**
 * @brief Get the state of a timer from RTEMS internal data.
 *
 * @param id The timer ID.
 *
 * @retval TIMER_INVALID The id argument is invalid.
 * @retval TIMER_INACTIVE The timer is not scheduled (i.e. it is
 *   new, run off, or canceled).
 * @retval TIMER_SCHEDULED The timer is scheduled.
 * @retval TIMER_PENDING The timer is pending.
 */
Timer_States GetTimerState( rtems_id id );

/**
 * @brief Mark the realtime clock as never set.
 *
 * This function manipulates RTEMS internal data structures to undo the
 * effect of rtems_clock_set(). If the clock is not set, the function has no
 * effect.
 */
void UnsetClock( void );

void FatalInitialExtension(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
);

typedef void ( *FatalHandler )(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
);

void SetFatalHandler( FatalHandler fatal, void *arg );

void SetTaskSwitchExtension( rtems_task_switch_extension task_switch );

typedef struct {
  uint32_t fatal;
  uint32_t thread_begin;
  uint32_t thread_create;
  uint32_t thread_delete;
  uint32_t thread_exitted;
  uint32_t thread_restart;
  uint32_t thread_start;
  uint32_t thread_switch;
  uint32_t thread_terminate;
} ExtensionCalls;

void ClearExtensionCalls( ExtensionCalls *calls );

void CopyExtensionCalls( const ExtensionCalls *from, ExtensionCalls *to );

void SetIORelaxHandler( void ( *handler )( void * ), void *arg );

void StartDelayThreadDispatch( uint32_t cpu_index );

void StopDelayThreadDispatch( uint32_t cpu_index );

bool AreInterruptsEnabled( void );

bool IsWhiteSpaceOnly( const char *s );

bool IsEqualIgnoreWhiteSpace( const char *a, const char *b );

#if defined(RTEMS_SMP)
bool TicketLockIsAvailable( const SMP_ticket_lock_Control *lock );

void TicketLockWaitForOwned( const SMP_ticket_lock_Control *lock );

void TicketLockWaitForOthers(
  const SMP_ticket_lock_Control *lock,
  unsigned int                   others
);

static inline bool ISRLockIsAvailable( const ISR_lock_Control *lock )
{
  return TicketLockIsAvailable( &lock->Lock.Ticket_lock );
}

static inline void ISRLockWaitForOwned( const ISR_lock_Control *lock )
{
  TicketLockWaitForOwned( &lock->Lock.Ticket_lock );
}

static inline void ISRLockWaitForOthers(
  const ISR_lock_Control *lock,
  unsigned int            others
)
{
  TicketLockWaitForOthers( &lock->Lock.Ticket_lock, others );
}
#endif

void *IdleBody( uintptr_t ignored );

/**
 * @brief This task configurations may be used to construct a task during
 *   tests.
 *
 * Only one task shall use this configuration at a time, otherwise two tasks
 * would share a stack.
 */
extern const rtems_task_config DefaultTaskConfig;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TX_SUPPORT_H */
