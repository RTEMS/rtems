/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the support functions for the validation
 *   test cases.
 *
 * The functions support the validation test cases.  Most of them call a
 * directive and check its status, so that a test case needs less code.  A
 * failure of a T_assert check ends the test case.  A failure of a T_quiet
 * check records the failure, and the test case continues.
 *
 * The functions serve these needs:
 *
 * - Tasks: CreateTask(), StartTask(), DeleteTask(), and the functions which
 *   get and set the priority, the home scheduler, and the processor affinity
 *   of a task.
 *
 * - Control of workers: SendEvents(), ReceiveAnyEvents(), and
 *   ReceiveAllEvents().
 *
 * - Task life: RestartTask(), RequestLifeChangesWithinISR(),
 *   SetCancelability(), and DisableCancelability().
 *
 * - Mutexes: CreateMutex(), ObtainMutex(), ReleaseMutex(), and DeleteMutex().
 *
 * - Time: ClockTick(), TimecounterTick(), SetTimecountCounter(),
 *   SetGetTimecountHandler(), UnsetClock(), GetTaskTimerInfo(), and
 *   DaysFromCivil().
 *
 * - Memory: MemoryAllocationFailWhen(), MemorySave(), and MemoryRestore().
 *
 * - Interrupts: CallWithinISR(), CallWithinISRSubmit(),
 *   GetTestableInterruptVector(), and RaiseSoftwareInterrupt().
 *
 * - Fatal errors and extensions: SetFatalHandler(), SetTaskSwitchExtension(),
 *   and ClearExtensionCalls().
 *
 * - SMP synchronization: WaitForExecutionStop(), WaitForIntendToBlock(),
 *   WaitForHeir(), and the ticket lock functions.
 *
 * - Test windows: SetFlag(), GetFlag(), WaitForFlag(), WaitForBlockedState(),
 *   and CallCounterObserve().
 *
 * A test case restores what it changed.  It restores the runner task with
 * RestoreRunnerMode(), RestoreRunnerPriority(), RestoreRunnerScheduler(), and
 * RestoreRunnerASR().  It removes a fatal handler, a thread switch extension,
 * and a timecount handler which it set.  It restores the memory areas which it
 * changed.
 *
 * The runner task starts each test case with the priority PRIO_DEFAULT, which
 * is one.  The values of Priority assume a runner with the priority
 * PRIO_NORMAL.  A worker with PRIO_HIGH or a higher priority then preempts the
 * runner, and a worker with PRIO_LOW or a lower priority does not.  A test
 * case with workers therefore sets the priority of the runner to PRIO_NORMAL
 * with SetSelfPriority() in its setup.  It restores the priority with
 * RestoreRunnerPriority() in its teardown.  TQInitialize() and TQDestroy() do
 * that for the thread queue tests.
 *
 * With the priority PRIO_DEFAULT, only PRIO_PSEUDO_ISR is higher than the
 * priority of the runner.  In uniprocessor configurations, a started worker
 * then runs only after the runner blocks or lowers its priority.
 *
 * In uniprocessor configurations, several wait functions return at once.  The
 * state which they wait for is then reached already.
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

/**
 * @brief This enumeration provides the task priorities of the validation
 *   tests.
 *
 * The values assume a runner task with the priority PRIO_NORMAL.  A lower
 * value is a higher priority.
 */
typedef enum {
  /**
   * @brief This priority is zero, which is higher than every task priority.
   */
  PRIO_PSEUDO_ISR,

  /**
   * @brief This priority is one, which is equal to PRIO_DEFAULT.
   */
  PRIO_VERY_ULTRA_HIGH,

  /**
   * @brief This priority is higher than PRIO_VERY_HIGH.
   */
  PRIO_ULTRA_HIGH,

  /**
   * @brief This priority is higher than PRIO_HIGH.
   */
  PRIO_VERY_HIGH,

  /**
   * @brief This priority is higher than PRIO_NORMAL.
   */
  PRIO_HIGH,

  /**
   * @brief This priority is the priority of the runner task in a test case
   *   with workers.
   */
  PRIO_NORMAL,

  /**
   * @brief This priority is lower than PRIO_NORMAL.
   */
  PRIO_LOW,

  /**
   * @brief This priority is lower than PRIO_LOW.
   */
  PRIO_VERY_LOW,

  /**
   * @brief This priority is lower than PRIO_VERY_LOW.
   */
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

/**
 * @brief Creates a task with the name and the priority.
 *
 * The first four characters of the name form the task name.  The macro calls
 * DoCreateTask().
 *
 * @param name is a string of at least four characters.
 * @param priority is the initial task priority.
 *
 * @return Returns the identifier of the created task.
 */
#define CreateTask( name, priority )                                \
  DoCreateTask(                                                     \
    rtems_build_name( name[ 0 ], name[ 1 ], name[ 2 ], name[ 3 ] ), \
    priority                                                        \
  )

#define SCHEDULER_A_ID 0xf010001

#define SCHEDULER_B_ID 0xf010002

#define SCHEDULER_C_ID 0xf010003

#define SCHEDULER_D_ID 0xf010004

/**
 * @brief Creates a task.
 *
 * The task has the minimum stack size of the tests, the default modes, and the
 * default attributes.  A failure of rtems_task_create() ends the test case.
 *
 * @param name is the task name.
 * @param priority is the initial task priority.
 *
 * @return Returns the identifier of the created task.
 */
rtems_id DoCreateTask( rtems_name name, rtems_task_priority priority );

/**
 * @brief Starts the task.
 *
 * A failure of rtems_task_start() ends the test case.
 *
 * @param id is the identifier of the task.
 * @param entry is the task entry.
 * @param arg is the argument of the task entry.
 */
void StartTask( rtems_id id, rtems_task_entry entry, void *arg );

/**
 * @brief Deletes the task.
 *
 * The function does nothing for an identifier of zero.  The test records a
 * failure, if rtems_task_delete() does not succeed.
 *
 * @param id is the identifier of the task, or zero.
 */
void DeleteTask( rtems_id id );

/**
 * @brief Suspends the task.
 *
 * The test records a failure, if rtems_task_suspend() does not succeed.
 *
 * @param id is the identifier of the task.
 */
void SuspendTask( rtems_id id );

/**
 * @brief Suspends the executing task.
 *
 * The test records a failure, if rtems_task_suspend() does not succeed.
 */
void SuspendSelf( void );

/**
 * @brief Resumes the task.
 *
 * The test records a failure, if rtems_task_resume() does not succeed.
 *
 * @param id is the identifier of the task.
 */
void ResumeTask( rtems_id id );

/**
 * @brief Checks whether the task is suspended.
 *
 * The test records a failure, if rtems_task_is_suspended() returns a status
 * other than RTEMS_SUCCESSFUL and RTEMS_ALREADY_SUSPENDED.
 *
 * @param id is the identifier of the task.
 *
 * @retval true The task is suspended.
 * @retval false The task is not suspended.
 */
bool IsTaskSuspended( rtems_id id );

/**
 * @brief Restarts the task.
 *
 * The test records a failure, if rtems_task_restart() does not succeed.
 *
 * @param id is the identifier of the task.
 * @param arg is the argument of the task entry.
 */
void RestartTask( rtems_id id, void *arg );

/**
 * @brief Requests life changes of the task within the interrupt service
 *   routine of the call within ISR support.
 *
 * The function issues both requests in one interrupt service routine.  The
 * restart request comes first.  The test records a failure, if
 * rtems_task_restart() or pthread_cancel() does not succeed.
 *
 * @param id is the identifier of the task.  pthread_self() returns this
 *   identifier in the task.
 * @param arg is the argument of the task entry of the restarted task.
 * @param restart is true, if the function shall request a restart of the
 *   task.
 * @param cancel is true, if the function shall request a cancellation of the
 *   task.
 */
void RequestLifeChangesWithinISR(
  rtems_id id,
  void    *arg,
  bool     restart,
  bool     cancel
);

/**
 * @brief Sets the cancel type and then the cancel state of the executing
 *   thread.
 *
 * The thread acts upon a pending request in this call, if the state is
 * PTHREAD_CANCEL_ENABLE and the type is PTHREAD_CANCEL_ASYNCHRONOUS.  The test
 * records a failure, if pthread_setcanceltype() or pthread_setcancelstate()
 * does not succeed.
 *
 * @param state is the cancel state to set.
 * @param type is the cancel type to set.
 */
void SetCancelability( int state, int type );

/**
 * @brief Sets the cancel state of the executing thread to
 *   PTHREAD_CANCEL_DISABLE and then its cancel type to
 *   PTHREAD_CANCEL_DEFERRED.
 *
 * A request of a later cancellation or restart of the thread stays pending.
 * The test records a failure, if pthread_setcancelstate() or
 * pthread_setcanceltype() does not succeed.
 *
 * @param[out] old_state is the pointer to an int object.  When the pointer is
 *   not NULL, the function stores the previous cancel state in the object.
 * @param[out] old_type is the pointer to an int object.  When the pointer is
 *   not NULL, the function stores the previous cancel type in the object.
 */
void DisableCancelability( int *old_state, int *old_type );

/**
 * @brief Gets the pending events of the executing task.
 *
 * The function receives no event.  The test records a failure, if
 * rtems_event_receive() does not succeed.
 *
 * @return Returns the set of pending events.
 */
rtems_event_set QueryPendingEvents( void );

/**
 * @brief Receives the pending events of the executing task without a wait.
 *
 * The function ignores the status of rtems_event_receive().
 *
 * @return Returns the received events, or zero if no event was pending.
 */
rtems_event_set PollAnyEvents( void );

/**
 * @brief Receives any events of the executing task.
 *
 * The function waits forever for at least one event.
 *
 * @return Returns the received events.
 */
rtems_event_set ReceiveAnyEvents( void );

/**
 * @brief Receives any events of the executing task with a timeout.
 *
 * The function ignores the status of rtems_event_receive().
 *
 * @param ticks is the timeout in clock ticks.
 *
 * @return Returns the received events, or zero if the timeout expired.
 */
rtems_event_set ReceiveAnyEventsTimed( rtems_interval ticks );

/**
 * @brief Receives all the events for the executing task.
 *
 * The function waits forever until all the events are pending.  The test
 * records a failure, if rtems_event_receive() does not succeed or receives
 * other events.
 *
 * @param events is the set of events to receive.
 */
void ReceiveAllEvents( rtems_event_set events );

/**
 * @brief Sends the events to the task.
 *
 * The test records a failure, if rtems_event_send() does not succeed.
 *
 * @param id is the identifier of the receiver task.
 * @param events is the set of events to send.
 */
void SendEvents( rtems_id id, rtems_event_set events );

/**
 * @brief Gets the mode of the executing task.
 *
 * @return Returns the current task mode.
 */
rtems_mode GetMode( void );

/**
 * @brief Sets the mode of the executing task.
 *
 * The test records a failure, if rtems_task_mode() does not succeed.
 *
 * @param set is the mode set.
 * @param mask is the mode mask.
 *
 * @return Returns the previous task mode.
 */
rtems_mode SetMode( rtems_mode set, rtems_mode mask );

/**
 * @brief Gets the priority of the task with respect to its home scheduler.
 *
 * @param id is the identifier of the task.
 *
 * @return Returns the current priority of the task.
 */
rtems_task_priority GetPriority( rtems_id id );

/**
 * @brief Gets the priority of the task with respect to the scheduler.
 *
 * @param task_id is the identifier of the task.
 * @param scheduler_id is the identifier of the scheduler.
 *
 * @return Returns the priority of the task, or PRIO_INVALID if
 *   rtems_task_get_priority() does not succeed.
 */
rtems_task_priority GetPriorityByScheduler(
  rtems_id task_id,
  rtems_id scheduler_id
);

/**
 * @brief Sets the priority of the task.
 *
 * The priority RTEMS_CURRENT_PRIORITY leaves the priority of the task as it
 * is.  The test records a failure, if rtems_task_set_priority() does not
 * succeed.
 *
 * @param id is the identifier of the task.
 * @param priority is the new priority.
 *
 * @return Returns the previous priority of the task.
 */
rtems_task_priority SetPriority( rtems_id id, rtems_task_priority priority );

/**
 * @brief Gets the priority of the executing task.
 *
 * @return Returns the current priority of the executing task.
 */
rtems_task_priority GetSelfPriority( void );

/**
 * @brief Sets the priority of the executing task.
 *
 * @param priority is the new priority.
 *
 * @return Returns the previous priority.
 */
rtems_task_priority SetSelfPriority( rtems_task_priority priority );

/**
 * @brief Sets the priority of the executing task without an implicit yield.
 *
 * The executing task owns a priority ceiling mutex while the priority changes.
 * A lower priority therefore causes no implicit yield of the processor.
 *
 * Use this function when a lower priority of the executing task shall not let
 * another task run before the next directive call.  The task delete and
 * restart tests lower the priority of a deleter this way before the delete.
 * See tc-task-delete.c.
 *
 * @param priority is the new priority.
 *
 * @return Returns the previous priority.
 */
rtems_task_priority SetSelfPriorityNoYield( rtems_task_priority priority );

/**
 * @brief Gets the home scheduler of the task.
 *
 * The test records a failure, if rtems_task_get_scheduler() does not succeed.
 *
 * @param id is the identifier of the task.
 *
 * @return Returns the identifier of the home scheduler.
 */
rtems_id GetScheduler( rtems_id id );

/**
 * @brief Gets the home scheduler of the executing task.
 *
 * @return Returns the identifier of the home scheduler.
 */
rtems_id GetSelfScheduler( void );

/**
 * @brief Sets the home scheduler and the priority of the task.
 *
 * The test records a failure, if rtems_task_set_scheduler() does not succeed.
 *
 * @param task_id is the identifier of the task.
 * @param scheduler_id is the identifier of the new home scheduler.
 * @param priority is the priority with respect to the new home scheduler.
 */
void SetScheduler(
  rtems_id            task_id,
  rtems_id            scheduler_id,
  rtems_task_priority priority
);

/**
 * @brief Sets the home scheduler and the priority of the executing task.
 *
 * @param scheduler_id is the identifier of the new home scheduler.
 * @param priority is the priority with respect to the new home scheduler.
 */
void SetSelfScheduler( rtems_id scheduler_id, rtems_task_priority priority );

/**
 * @brief Gets the processor affinity of the task.
 *
 * The test records a failure, if rtems_task_get_affinity() does not succeed.
 *
 * @param id is the identifier of the task.
 * @param set is the processor set which receives the affinity.
 */
void GetAffinity( rtems_id id, cpu_set_t *set );

/**
 * @brief Gets the processor affinity of the executing task.
 *
 * @param set is the processor set which receives the affinity.
 */
void GetSelfAffinity( cpu_set_t *set );

/**
 * @brief Sets the processor affinity of the task.
 *
 * The test records a failure, if rtems_task_set_affinity() does not succeed.
 *
 * @param id is the identifier of the task.
 * @param set is the processor set of the new affinity.
 */
void SetAffinity( rtems_id id, const cpu_set_t *set );

/**
 * @brief Sets the processor affinity of the executing task.
 *
 * @param set is the processor set of the new affinity.
 */
void SetSelfAffinity( const cpu_set_t *set );

/**
 * @brief Sets the processor affinity of the task to one processor.
 *
 * @param id is the identifier of the task.
 * @param cpu_index is the index of the processor.
 */
void SetAffinityOne( rtems_id id, uint32_t cpu_index );

/**
 * @brief Sets the processor affinity of the executing task to one processor.
 *
 * @param cpu_index is the index of the processor.
 */
void SetSelfAffinityOne( uint32_t cpu_index );

/**
 * @brief Sets the processor affinity of the task to all processors.
 *
 * @param id is the identifier of the task.
 */
void SetAffinityAll( rtems_id id );

/**
 * @brief Sets the processor affinity of the executing task to all processors.
 */
void SetSelfAffinityAll( void );

/**
 * @brief Yields the processor of the executing task.
 *
 * The function calls rtems_task_wake_after() with RTEMS_YIELD_PROCESSOR.
 */
void Yield( void );

/**
 * @brief Yields the processor of the task.
 *
 * The function calls _Thread_Yield() for the task while thread dispatching is
 * disabled.  It does nothing for an invalid identifier.
 *
 * Use this function when the runner moves to another scheduler and a worker
 * shall give up its processor to a waiting helper.  See tr-tq-surrender-mrsp.c
 * and tr-tq-surrender-priority-inherit.c.
 *
 * @param id is the identifier of the task.
 */
void YieldTask( rtems_id id );

/**
 * @brief Adds the processor to the scheduler.
 *
 * The test records a failure, if rtems_scheduler_add_processor() does not
 * succeed.
 *
 * @param scheduler_id is the identifier of the scheduler.
 * @param cpu_index is the index of the processor.
 */
void AddProcessor( rtems_id scheduler_id, uint32_t cpu_index );

/**
 * @brief Removes the processor from the scheduler.
 *
 * The test records a failure, if rtems_scheduler_remove_processor() does not
 * succeed.
 *
 * @param scheduler_id is the identifier of the scheduler.
 * @param cpu_index is the index of the processor.
 */
void RemoveProcessor( rtems_id scheduler_id, uint32_t cpu_index );

/**
 * @brief Creates a mutex with the priority inheritance locking protocol.
 *
 * The mutex is a binary semaphore with the priority discipline.  It is
 * available after the creation.  The test records a failure, if
 * rtems_semaphore_create() does not succeed.
 *
 * @return Returns the identifier of the mutex.
 */
rtems_id CreateMutex( void );

/**
 * @brief Creates a mutex without a locking protocol.
 *
 * The mutex is a binary semaphore with the priority discipline.  It is
 * available after the creation.  The test records a failure, if
 * rtems_semaphore_create() does not succeed.
 *
 * @return Returns the identifier of the mutex.
 */
rtems_id CreateMutexNoProtocol( void );

/**
 * @brief Creates a mutex with the FIFO discipline.
 *
 * The mutex is a binary semaphore without a locking protocol.  It is available
 * after the creation.  The test records a failure, if rtems_semaphore_create()
 * does not succeed.
 *
 * @return Returns the identifier of the mutex.
 */
rtems_id CreateMutexFIFO( void );

/**
 * @brief Checks whether the executing task owns the mutex.
 *
 * @param id is the identifier of the mutex.
 *
 * @retval true The executing task owns the mutex.
 * @retval false Another task or no task owns the mutex, or the identifier is
 *   invalid.
 */
bool IsMutexOwner( rtems_id id );

/**
 * @brief Deletes the mutex.
 *
 * The function does nothing for INVALID_ID.  The test records a failure, if
 * rtems_semaphore_delete() does not succeed.
 *
 * @param id is the identifier of the mutex, or INVALID_ID.
 */
void DeleteMutex( rtems_id id );

/**
 * @brief Obtains the mutex.
 *
 * The function waits forever.  The test records a failure, if
 * rtems_semaphore_obtain() does not succeed.
 *
 * @param id is the identifier of the mutex.
 */
void ObtainMutex( rtems_id id );

/**
 * @brief Obtains the mutex with a timeout.
 *
 * The test records a failure, if rtems_semaphore_obtain() does not succeed.
 *
 * @param id is the identifier of the mutex.
 * @param ticks is the timeout in clock ticks.
 */
void ObtainMutexTimed( rtems_id id, rtems_interval ticks );

/**
 * @brief Obtains the mutex and expects a deadlock.
 *
 * The test records a failure, if rtems_semaphore_obtain() does not return
 * RTEMS_INCORRECT_STATE.
 *
 * @param id is the identifier of the mutex.
 */
void ObtainMutexDeadlock( rtems_id id );

/**
 * @brief Releases the mutex.
 *
 * The test records a failure, if rtems_semaphore_release() does not succeed.
 *
 * @param id is the identifier of the mutex.
 */
void ReleaseMutex( rtems_id id );

struct Thread_queue_Queue;

/**
 * @brief Gets the thread queue of the mutex.
 *
 * @param id is the identifier of the mutex.
 *
 * @return Returns the thread queue of the mutex, or NULL for an invalid
 *   identifier.
 */
struct Thread_queue_Queue *GetMutexThreadQueue( rtems_id id );

/**
 * @brief Removes the asynchronous signal routine of the runner task.
 *
 * The function calls rtems_signal_catch() for the executing task.  The test
 * records a failure, if rtems_signal_catch() does not succeed.
 */
void RestoreRunnerASR( void );

/**
 * @brief Restores the default mode of the runner task.
 *
 * The function sets all modes of the executing task to RTEMS_DEFAULT_MODES.
 * The test records a failure, if rtems_task_mode() does not succeed.
 */
void RestoreRunnerMode( void );

/**
 * @brief Restores the priority of the runner task.
 *
 * The function sets the priority of the executing task to PRIO_DEFAULT.
 *
 * Call this function in the teardown of a test case which set the priority of
 * the runner to PRIO_NORMAL in its setup.  See the overview of this file.
 */
void RestoreRunnerPriority( void );

/**
 * @brief Restores the home scheduler of the runner task.
 *
 * The function moves the executing task to the scheduler with the identifier
 * SCHEDULER_A_ID and the priority PRIO_DEFAULT.
 */
void RestoreRunnerScheduler( void );

struct _Thread_Control;

/**
 * @brief Gets the thread control block of the task.
 *
 * @param id is the identifier of the task.
 *
 * @return Returns the thread control block, or NULL for an invalid identifier.
 */
struct _Thread_Control *GetThread( rtems_id id );

/**
 * @brief Gets the thread control block of the executing thread.
 *
 * @return Returns the thread control block of the executing thread.
 */
struct _Thread_Control *GetExecuting( void );

/**
 * @brief Checks whether the object is in the thread-local storage area of the
 *   task.
 *
 * @param id is the identifier of the task.
 * @param obj is the address of the object.
 *
 * @retval true The object is in the thread-local storage area of the task.
 * @retval false The object is not in this area, or the identifier is invalid.
 */
bool IsTLSObjectOfThread( rtems_id id, const void *obj );

/**
 * @brief Runs the timeout of the thread.
 *
 * The routine hands the token which the watchdog of the thread carries, as
 * _Watchdog_Do_tickle() does.
 *
 * @param thread is the thread.
 */
void TQTimeout( struct _Thread_Control *thread );

/**
 * @brief Frees the resources of the zombie threads.
 *
 * The function calls _Thread_Kill_zombies() while it owns the object allocator
 * mutex.
 *
 * Use this function after the delete of a task, when the test checks the
 * thread extension calls or the release of the thread resources.  See
 * tc-userext.c.
 */
void KillZombies( void );

/**
 * @brief Waits until the task executes on no processor.
 *
 * The wait has no bound.  In uniprocessor configurations, the function returns
 * at once.
 *
 * Use this function in SMP tests after an event send to a worker on another
 * processor, before the test checks the state of the worker.  Without the
 * wait, the worker may still execute its request.
 * TQSendAndWaitForExecutionStop() combines the send and the wait.
 *
 * @param task_id is the identifier of the task.
 */
void WaitForExecutionStop( rtems_id task_id );

/**
 * @brief Waits until the task intends to block on a thread queue.
 *
 * The function waits until the wait class of the task is
 * THREAD_WAIT_CLASS_QUEUE.  The wait has no bound.  In uniprocessor
 * configurations, the function returns at once.
 *
 * Use this function in SMP tests when the worker shall be inside a blocking
 * directive and the test acts before the block.  See tc-sem-smp.c, which
 * checks the priority of a waiting task.
 *
 * @param task_id is the identifier of the task.
 */
void WaitForIntendToBlock( rtems_id task_id );

/**
 * @brief Waits until the task is the heir of the processor.
 *
 * The wait has no bound.
 *
 * Use this function when a scheduler change of the runner selects a task on
 * another processor and the test shall continue after the dispatch decision.
 * See tr-tq-surrender-mrsp.c.
 *
 * @param cpu_index is the index of the processor.
 * @param task_id is the identifier of the task.
 */
void WaitForHeir( uint32_t cpu_index, rtems_id task_id );

/**
 * @brief Waits until another task is the heir of the processor and the
 *   processor enables thread dispatching.
 *
 * The wait has no bound.
 *
 * The performance tests run a worker on processor one.  They call this
 * function after each measured operation, so that the next sample starts after
 * the worker left the processor.  See tc-event-performance.c.
 *
 * @param cpu_index is the index of the processor.
 * @param task_id is the identifier of the current heir.
 */
void WaitForNextTask( uint32_t cpu_index, rtems_id task_id );

/**
 * @brief Sets the flag of a test window.
 *
 * The release store publishes every write which the caller made before it to
 * a party which reads the flag with GetFlag().
 *
 * @param flag is the flag.
 * @param value is the value to set.
 */
static inline void SetFlag( Atomic_Uint *flag, unsigned int value )
{
  _Atomic_Store_uint( flag, value, ATOMIC_ORDER_RELEASE );
}

/**
 * @brief Gets the flag of a test window.
 *
 * The acquire load pairs with the release store of SetFlag(), so the caller
 * observes every write which the setter made before it.
 *
 * @param flag is the flag.
 *
 * @return Returns the value of the flag.
 */
static inline unsigned int GetFlag( const Atomic_Uint *flag )
{
  return _Atomic_Load_uint( flag, ATOMIC_ORDER_ACQUIRE );
}

/**
 * @brief Checks whether the bound of a wait of a test window expired.
 *
 * A wait which needs a predicate of its own uses this check.  WaitForFlag()
 * and WaitForBlockedState() serve the two common predicates and apply the
 * bound themselves.
 *
 * @param begin is the monotonic time point at which the wait began.
 *
 * @retval true The bound expired.
 * @retval false The wait may go on.
 */
bool WaitTimedOut( int64_t begin );

/**
 * @brief Waits until the flag is set or the bound of the wait expires.
 *
 * A wait of a test window can run in the tickle of the processor which counts
 * the ticks.  A monotonic time point and not the tick counter therefore bounds
 * the wait.  The bound is one second.
 *
 * @param flag is the flag to wait for.
 *
 * @retval true The flag is set.
 * @retval false The bound of the wait expired.
 */
bool WaitForFlag( const Atomic_Uint *flag );

/**
 * @brief Waits until the task reaches one of the states, the flag is set, or
 *   the bound of the wait expires.
 *
 * The wait ends with the block of the task and not with the entry of the task
 * into a directive, so a task which reaches no block has the time to end the
 * directive.  The bound is one second.
 *
 * @param task_id is the identifier of the task.
 * @param states is the set of states to wait for.
 * @param flag is the flag which ends the wait as well.
 *
 * @retval true The task reached one of the states or the flag is set.
 * @retval false The bound of the wait expired.
 */
bool WaitForBlockedState(
  rtems_id           task_id,
  uint32_t           states,
  const Atomic_Uint *flag
);

/**
 * @brief This enumeration provides the states of the timer of a task.
 *
 * See GetTaskTimerInfo().
 */
typedef enum {
  /**
   * @brief The task identifier or the thread is invalid.
   */
  TASK_TIMER_INVALID,

  /**
   * @brief The timer of the task is not scheduled.
   */
  TASK_TIMER_INACTIVE,

  /**
   * @brief The timer of the task is scheduled in clock ticks.
   */
  TASK_TIMER_TICKS,

  /**
   * @brief The timer of the task is scheduled with respect to
   *   CLOCK_REALTIME.
   */
  TASK_TIMER_REALTIME,

  /**
   * @brief The timer of the task is scheduled with respect to
   *   CLOCK_MONOTONIC.
   */
  TASK_TIMER_MONOTONIC
} TaskTimerState;

/**
 * @brief This structure provides the timer information of a task.
 *
 * See GetTaskTimerInfo().
 */
typedef struct {
  /**
   * @brief This member contains the state of the timer.
   */
  TaskTimerState state;

  /**
   * @brief This member contains the expire time point of the timer in the
   *   unit of its watchdog header.
   */
  uint64_t expire_ticks;

  /**
   * @brief This member contains the expire time point of a timer of
   *   CLOCK_REALTIME or CLOCK_MONOTONIC.
   *
   * For other states, both members of the timespec are minus one.
   */
  struct timespec expire_timespec;
} TaskTimerInfo;

/**
 * @brief Gets the timer information of the task.
 *
 * An invalid identifier yields the state TASK_TIMER_INVALID.
 *
 * Use this function to check that a directive armed the timeout of a task with
 * the expected clock and expire time point.  See tc-task-wake-when.c and
 * tc-clock-nanosleep.c.
 *
 * @param id is the identifier of the task.
 * @param info is the structure which receives the information.
 */
void GetTaskTimerInfo( rtems_id id, TaskTimerInfo *info );

/**
 * @brief Gets the timer information of the thread.
 *
 * A thread of NULL yields the state TASK_TIMER_INVALID.  The information
 * contains the expire time point in clock ticks.  For a timer of
 * CLOCK_REALTIME or CLOCK_MONOTONIC, it contains the expire time point as a
 * timespec as well.
 *
 * Use this function instead of GetTaskTimerInfo() when the identifier of the
 * task may be invalid at the time of the check.  The task delete and restart
 * tests use it in a thread extension.  See tc-task-restart.c.
 *
 * @param thread is the thread, or NULL.
 * @param info is the structure which receives the information.
 */
void GetTaskTimerInfoByThread(
  struct _Thread_Control *thread,
  TaskTimerInfo          *info
);

/**
 * @brief Simulates a clock tick.
 *
 * The function runs the watchdog tick on every processor.  It changes neither
 * CLOCK_MONOTONIC nor CLOCK_REALTIME.
 *
 * Tests without a clock driver use this function to fire watchdog timeouts at
 * a known point.  The timer tests call it in a loop until the timer service
 * routine runs.  See tc-timer-fire-after.c.
 */
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

/**
 * @brief This type represents a handler which returns the timecount of the
 *   software timecounter.
 *
 * See SetGetTimecountHandler().
 */
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
 * @brief This constant represents the amount of binary time units per software
 *   timecounter tick.
 */
#define SOFTWARE_TIMECOUNTER_INTERVAL 4295

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

/**
 * @brief This structure contains the saved state of the memory areas of the
 *   system.
 *
 * See MemorySave() and MemoryRestore().
 */
typedef struct {
  /**
   * @brief This member contains the saved state of each memory area.
   */
  struct {
    /**
     * @brief This member contains the begin of the memory area.
     */
    const void *begin;

    /**
     * @brief This member contains the begin of the free space of the memory
     *   area.
     */
    void *free_begin;

    /**
     * @brief This member contains the end of the memory area.
     */
    const void *end;
  } areas[ 2 ];

  /**
   * @brief This member contains the count of saved memory areas.
   */
  size_t count;
} MemoryContext;

/**
 * @brief Saves the state of the memory areas of the system.
 *
 * The function saves the begin, the free begin, and the end of each memory
 * area.  The system shall have at most two memory areas, otherwise the test
 * case ends.
 *
 * The memory allocation tests save the state in their setup and restore it
 * with MemoryRestore() later.  A test which lets directives allocate from the
 * memory areas keeps the areas of the other tests intact this way.  See
 * tc-mem-rtems-calloc.c.
 *
 * @param ctx is the context which receives the state.
 */
void MemorySave( MemoryContext *ctx );

/**
 * @brief Restores the state of the memory areas of the system.
 *
 * The function restores the begin, the free begin, and the end of each memory
 * area which MemorySave() saved.
 *
 * Call this function with the context which the setup of the test case filled.
 * See MemorySave().
 *
 * @param ctx is the context which MemorySave() filled.
 */
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

/**
 * @brief This structure represents a request to call a handler within the
 *   interrupt service routine of the call within ISR support.
 *
 * See CallWithinISRSubmit() and CallWithinISRWait().
 */
typedef struct {
  /**
   * @brief This member is the node of the request in the pending requests.
   */
  Chain_Node node;

  /**
   * @brief This member contains the handler to call.
   */
  void ( *handler )( void * );

  /**
   * @brief This member contains the argument of the handler.
   */
  void *arg;

  /**
   * @brief This member is set after the call of the handler.
   */
  Atomic_Uint done;
} CallWithinISRRequest;

/**
 * @brief Calls the handler within the interrupt service routine of the call
 *   within ISR support.
 *
 * The function submits a request and waits for its completion.
 *
 * @param handler is the handler to call.
 * @param arg is the argument of the handler.
 */
void CallWithinISR( void ( *handler )( void * ), void *arg );

/**
 * @brief Submits the request to call a handler within the interrupt service
 *   routine.
 *
 * The function appends the request to the pending requests and raises the
 * interrupt.  The request shall exist until CallWithinISRWait() returns.
 *
 * Use this function when the caller shall not wait for the interrupt, for
 * example in a scheduler event handler.  The flush tests submit a request in
 * the block operation of a worker, so the flush runs within an interrupt.  See
 * tr-tq-flush-fifo.c.
 *
 * @param request is the request.
 */
void CallWithinISRSubmit( CallWithinISRRequest *request );

/**
 * @brief Waits for the completion of the request.
 *
 * A request which does not complete within a bounded count of spins ends the
 * test case.
 *
 * @param request is the request.
 */
void CallWithinISRWait( const CallWithinISRRequest *request );

/**
 * @brief Raises the interrupt of the call within ISR support.
 */
void CallWithinISRRaise( void );

/**
 * @brief Clears the interrupt of the call within ISR support.
 */
void CallWithinISRClear( void );

/**
 * @brief Gets the interrupt vector of the call within ISR support.
 *
 * @return Returns the interrupt vector number, or UINT32_MAX if no vector has
 *   the handler of the support installed.
 */
rtems_vector_number CallWithinISRGetVector( void );

/**
 * @brief Gets the alternative software interrupt vector.
 *
 * @return Returns TM27_INTERRUPT_VECTOR_ALTERNATIVE, or UINT32_MAX if the BSP
 *   provides no alternative vector.
 */
rtems_vector_number GetSoftwareInterruptVector( void );

/**
 * @brief This structure contains the state of a thread queue wrapper.
 *
 * See WrapThreadQueueInitialize().
 */
typedef struct {
  /**
   * @brief This member contains the thread queue operations of the wrapper.
   */
  Thread_queue_Operations tq_ops;

  /**
   * @brief This member references the thread queue operations which the
   *   wrapper replaced, or is NULL if the thread waits on the thread queue of
   *   the wrapper.
   */
  const Thread_queue_Operations *wrapped_ops;

  /**
   * @brief This member contains the thread queue of the wrapper.
   */
  Thread_queue_Control thread_queue;

  /**
   * @brief This member contains the request which calls the handler of the
   *   wrapper.
   */
  CallWithinISRRequest isr_request;
} WrapThreadQueueContext;

/**
 * @brief Initializes the context of a thread queue wrapper.
 *
 * @param ctx is the context of the wrapper.
 * @param handler is the handler which the wrapper calls in the extract
 *   operation.
 * @param arg is the argument of the handler.
 */
void WrapThreadQueueInitialize(
  WrapThreadQueueContext *ctx,
  void ( *handler )( void * ),
  void *arg
);

/**
 * @brief Wraps the extract operation of the thread so that it calls the
 *   handler within an interrupt.
 *
 * If the thread waits on a thread queue, the wrapper replaces the thread queue
 * operations of the thread.  The wrapped extract calls the original extract
 * operation afterwards.  Otherwise, the thread waits on the thread queue of
 * the wrapper.
 *
 * Use this function to run a handler in the middle of a thread queue extract.
 * See tc-task-restart.c, which issues a nested restart request this way.  Call
 * WrapThreadQueueDestroy() at the end of the test case.
 *
 * @param ctx is the context of the wrapper.
 * @param thread is the thread.
 */
void WrapThreadQueueExtract(
  WrapThreadQueueContext *ctx,
  struct _Thread_Control *thread
);

/**
 * @brief Wraps the extract operation of the thread so that it calls the
 *   handler directly.
 *
 * If the thread waits on a thread queue, the wrapper replaces the thread queue
 * operations of the thread.  The wrapped extract calls the original extract
 * operation afterwards.  Otherwise, the thread waits on the thread queue of
 * the wrapper.
 *
 * Use this function when the handler shall run on the processor of the
 * extract.  See tc-score-tq-smp.c, which changes the priority of a thread in
 * parallel to its extract.
 *
 * @param ctx is the context of the wrapper.
 * @param thread is the thread.
 */
void WrapThreadQueueExtractDirect(
  WrapThreadQueueContext *ctx,
  Thread_Control         *thread
);

/**
 * @brief Destroys the thread queue of the wrapper.
 *
 * @param ctx is the context of the wrapper.
 */
void WrapThreadQueueDestroy( WrapThreadQueueContext *ctx );

/**
 * @brief This structure counts the calls of a function which one task makes.
 *
 * A link time wrapper of the function adds to the counter.  The test arms the
 * counter for one task, runs the directive, and reads how often the directive
 * called the function.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of the task which the counter
   *   observes, or zero where it observes none.
   */
  Atomic_Uint task;

  /**
   * @brief This member contains the count of the calls.
   */
  Atomic_Uint count;
} CallCounter;

/**
 * @brief Arms the counter for the task and clears the count.
 *
 * @param counter is the counter.
 * @param task_id is the identifier of the task to observe.  A zero disarms
 *   the counter, which every teardown shall do.
 */
void CallCounterObserve( CallCounter *counter, rtems_id task_id );

/**
 * @brief Adds one call to the counter where the executing task is the one
 *   which the counter observes.
 *
 * A wrapper of the observed function calls the real function and then this,
 * so the count states that the call returned.  A wrapper which counts first
 * states that a call which still blocks already happened.
 *
 * @warning The counter counts calls and not critical sections.  A recursive
 *   lock adds one count per nesting level.  A test which reads the count of
 *   such a lock as a count of critical sections reads it wrong.
 *
 * @param counter is the counter.
 */
void CallCounterAdd( CallCounter *counter );

/**
 * @brief Gets the count of the calls.
 *
 * @param counter is the counter.
 *
 * @return Returns the count since the arm of the counter.
 */
unsigned int CallCounterGet( const CallCounter *counter );

/**
 * @brief This counter observes the obtains of the object allocator mutex.
 *
 * A directive which releases the mutex and obtains it again counts twice.  A
 * directive which holds it counts once.  The suite of the caller shall carry
 * the link flag ``-Wl,--wrap=_RTEMS_Lock_allocator``.
 *
 * The object allocator mutex is recursive, so a directive which obtains it
 * twice in a nest counts twice as well.  No directive of the tree does that
 * today.  A test which uses this counter shall check that claim for its
 * directive.
 */
extern CallCounter AllocatorLockCounter;

struct Per_CPU_Control;

/**
 * @brief Sets a handler for the next preemption intervention on the processor.
 *
 * The function adds a helper thread to the threads in need for help of the
 * processor.  The processor calls the handler once, in the next preemption
 * intervention of a thread dispatch.  In uniprocessor configurations, the
 * function does nothing.
 *
 * Use this function to run code in the thread dispatch which follows a
 * directive call on the processor.  See tc-task-construct.c.
 *
 * @param cpu is the processor.
 * @param handler is the handler.
 * @param arg is the argument of the handler.
 */
void SetPreemptionIntervention(
  struct Per_CPU_Control *cpu,
  void ( *handler )( void * ),
  void *arg
);

/**
 * @brief Gets the first interrupt vector with the required attributes.
 *
 * @param required is the set of required attributes.
 *
 * @return Returns the vector number, or BSP_INTERRUPT_VECTOR_COUNT if no
 *   vector has the attributes.
 */
rtems_vector_number GetValidInterruptVectorNumber(
  const rtems_interrupt_attributes *required
);

/**
 * @brief Gets an interrupt vector which a test may enable, disable, and raise.
 *
 * The vector is maskable, has the required attributes, and has no handler
 * installed.  It can be enabled and disabled.  If no such vector exists, the
 * function returns the alternative software interrupt vector.
 *
 * Use this vector to test the interrupt manager directives without an effect
 * of installed handlers.  See tc-intr-entry-install.c.
 *
 * @param required is the set of required attributes.
 *
 * @return Returns the vector number.
 */
rtems_vector_number GetTestableInterruptVector(
  const rtems_interrupt_attributes *required
);

/**
 * @brief Raises the interrupt.
 *
 * For the alternative software interrupt vector, the function uses the tm27
 * support of the BSP.  Otherwise, it calls rtems_interrupt_raise().
 *
 * @param vector is the interrupt vector number.
 *
 * @return Returns the status of the raise.
 */
rtems_status_code RaiseSoftwareInterrupt( rtems_vector_number vector );

/**
 * @brief Clears the interrupt.
 *
 * For the alternative software interrupt vector, the function uses the tm27
 * support of the BSP.  Otherwise, it calls rtems_interrupt_clear().
 *
 * @param vector is the interrupt vector number.
 *
 * @return Returns the status of the clear.
 */
rtems_status_code ClearSoftwareInterrupt( rtems_vector_number vector );

/**
 * @brief Checks whether handlers are installed for the interrupt vector.
 *
 * The test records a failure, if rtems_interrupt_handler_iterate() does not
 * succeed.
 *
 * @param vector is the interrupt vector number.
 *
 * @retval true At least one handler is installed.
 * @retval false No handler is installed.
 */
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
  void                             *user_data;
  /**
   * @brief This member contains the timer interval in ticks or seconds.
   */
  Watchdog_Interval                 interval;
} Timer_Scheduling_Data;

/**
 * @brief Get data related to scheduling a timer service routine
 *   from RTEMS internal structures.
 *
 * @param id The timer ID.
 * @param[out] data If the reference is not NULL, the data retrieved from
 *   internal RTEMS structures is stored here.
 */
void GetTimerSchedulingData( rtems_id id, Timer_Scheduling_Data *data );

/**
 * @brief The various states of a timer.
 */
typedef enum {
  /**
   * @brief The timer identifier is invalid.
   */
  TIMER_INVALID,

  /**
   * @brief The timer is not scheduled.
   */
  TIMER_INACTIVE,

  /**
   * @brief The timer is scheduled.
   */
  TIMER_SCHEDULED,

  /**
   * @brief The timer fired, and its timer service routine waits for the
   *   timer server.
   */
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

/**
 * @brief Gets the days of the date since the Epoch.
 *
 * The function uses the proleptic Gregorian calendar.  It implements
 * days_from_civil() of Howard Hinnant, "chrono-Compatible Low-Level Date
 * Algorithms".  The function does not use the date conversions of RTEMS or of
 * the C library.  Use it to derive the expected values of a date conversion.
 *
 * @param year is the year.  The year 0 is the year 1 BC.
 *
 * @param month is the month of the year, from 1 for January to 12 for
 *   December.
 *
 * @param day is the day of the month.  A value outside of the month counts
 *   from the first day of the month.
 *
 * @return Returns the days of the date since 1970-01-01.
 */
int64_t DaysFromCivil( int64_t year, unsigned int month, int64_t day );

/**
 * @brief Calls the fatal handler which SetFatalHandler() set.
 *
 * A test suite uses this function as its fatal extension of the initial
 * extensions.  The test records a failure, if always_set_to_false is true.
 *
 * @param source is the fatal source.
 * @param always_set_to_false is the parameter of the fatal extension which is
 *   always false.
 * @param code is the fatal code.
 */
void FatalInitialExtension(
  rtems_fatal_source source,
  bool               always_set_to_false,
  rtems_fatal_code   code
);

/**
 * @brief This type represents a handler which FatalInitialExtension() calls.
 *
 * See SetFatalHandler().
 */
typedef void ( *FatalHandler )(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
);

/**
 * @brief Sets the handler which FatalInitialExtension() calls.
 *
 * Use this function to catch an expected fatal error.  The handler usually
 * returns to the test with longjmp().  Call this function with NULL
 * afterwards.  See TQEnqueueFatal() and tc-task-restart.c.
 *
 * @param fatal is the fatal handler, or NULL.
 * @param arg is the argument of the fatal handler.
 */
void SetFatalHandler( FatalHandler fatal, void *arg );

/**
 * @brief Sets the thread switch extension.
 *
 * The function creates the extension set for the first extension and deletes
 * it for NULL.  The test records a failure, if the create or the delete of the
 * extension set does not succeed.
 *
 * Use this function to observe the thread switches of a test action.  Call it
 * with NULL afterwards.  See tc-score-thread.c.
 *
 * @param task_switch is the thread switch extension, or NULL.
 */
void SetTaskSwitchExtension( rtems_task_switch_extension task_switch );

/**
 * @brief This structure contains a count for each kind of extension call.
 *
 * A test case counts the extension calls in its extensions.  See
 * ClearExtensionCalls() and CopyExtensionCalls().
 */
typedef struct {
  /**
   * @brief This member contains the count of fatal extension calls.
   */
  uint32_t fatal;

  /**
   * @brief This member contains the count of thread begin extension calls.
   */
  uint32_t thread_begin;

  /**
   * @brief This member contains the count of thread create extension calls.
   */
  uint32_t thread_create;

  /**
   * @brief This member contains the count of thread delete extension calls.
   */
  uint32_t thread_delete;

  /**
   * @brief This member contains the count of thread exitted extension calls.
   */
  uint32_t thread_exitted;

  /**
   * @brief This member contains the count of thread restart extension calls.
   */
  uint32_t thread_restart;

  /**
   * @brief This member contains the count of thread start extension calls.
   */
  uint32_t thread_start;

  /**
   * @brief This member contains the count of thread switch extension calls.
   */
  uint32_t thread_switch;

  /**
   * @brief This member contains the count of thread terminate extension
   *   calls.
   */
  uint32_t thread_terminate;
} ExtensionCalls;

/**
 * @brief Clears the counts of the extension calls.
 *
 * @param calls is the counts of the extension calls.
 */
void ClearExtensionCalls( ExtensionCalls *calls );

/**
 * @brief Copies the counts of the extension calls.
 *
 * @param from is the source of the counts.
 * @param to is the destination of the counts.
 */
void CopyExtensionCalls( const ExtensionCalls *from, ExtensionCalls *to );

/**
 * @brief Sets the handler which the wrapper of _IO_Relax() calls.
 *
 * The wrapper calls the handler before the real _IO_Relax().
 *
 * The test suite shall link with the wrapper of _IO_Relax().  See
 * tc-dev-grlib-io.c, where the handler sets a device status bit so that a
 * polled output loop ends.
 *
 * @param handler is the handler, or NULL.
 * @param arg is the argument of the handler.
 */
void SetIORelaxHandler( void ( *handler )( void * ), void *arg );

/**
 * @brief Starts to delay the thread dispatch on the processor.
 *
 * The function submits a job to the processor.  The job delays the thread
 * dispatch until StopDelayThreadDispatch().  In uniprocessor configurations
 * and for a processor beyond the configured maximum, the function does
 * nothing.
 *
 * Use this function to keep processor one busy while the runner calls a
 * directive which targets a task on processor one.  The test then observes the
 * state before the thread dispatch of that task.  See tc-signal-send.c.
 *
 * @param cpu_index is the index of the processor.
 */
void StartDelayThreadDispatch( uint32_t cpu_index );

/**
 * @brief Stops the delay of the thread dispatch on the processor.
 *
 * The function waits for the completion of the job which
 * StartDelayThreadDispatch() submitted.
 *
 * @param cpu_index is the index of the processor.
 */
void StopDelayThreadDispatch( uint32_t cpu_index );

/**
 * @brief Checks whether interrupts are enabled on the executing processor.
 *
 * @retval true Interrupts are enabled.
 * @retval false Interrupts are disabled.
 */
bool AreInterruptsEnabled( void );

/**
 * @brief Checks whether the string contains only white space.
 *
 * @param s is the string.
 *
 * @retval true The string is empty or contains only white space.
 * @retval false The string contains other characters.
 */
bool IsWhiteSpaceOnly( const char *s );

/**
 * @brief Checks whether the strings are equal if white space is ignored.
 *
 * @param a is the first string.
 * @param b is the second string.
 *
 * @retval true The strings are equal.
 * @retval false The strings are not equal.
 */
bool IsEqualIgnoreWhiteSpace( const char *a, const char *b );

#if defined( RTEMS_SMP )
/**
 * @brief Checks whether the ticket lock is available.
 *
 * @param lock is the ticket lock.
 *
 * @retval true No processor owns the lock.
 * @retval false A processor owns the lock.
 */
bool TicketLockIsAvailable( const SMP_ticket_lock_Control *lock );

/**
 * @brief Waits until a processor owns the ticket lock.
 *
 * The wait has no bound.
 *
 * @param lock is the ticket lock.
 */
void TicketLockWaitForOwned( const SMP_ticket_lock_Control *lock );

/**
 * @brief Waits until the count of processors waits for the ticket lock.
 *
 * The count excludes the owner of the lock.  The wait has no bound.
 *
 * Use this function to wait until the other processors spin on a lock which
 * the test owns.  The order of the acquisitions is then known when the test
 * releases the lock.  See tc-score-tq-smp.c.
 *
 * @param lock is the ticket lock.
 * @param others is the count of processors which wait for the lock.
 */
void TicketLockWaitForOthers(
  const SMP_ticket_lock_Control *lock,
  unsigned int                   others
);

/**
 * @brief This structure contains the state of a ticket lock.
 *
 * See TicketLockGetState().
 */
typedef struct {
  /**
   * @brief This member references the ticket lock.
   */
  const SMP_ticket_lock_Control *lock;

  /**
   * @brief This member contains the next ticket of the lock at the time of
   *   TicketLockGetState().
   */
  unsigned int next_ticket;
} TicketLockState;

/**
 * @brief Gets the state of the ticket lock.
 *
 * TicketLockWaitForAcquires() and TicketLockWaitForReleases() use the state.
 *
 * Take the state before an action which acquires and releases the lock on
 * another processor.  Then wait for the release with
 * TicketLockWaitForReleases().  See tc-score-smp-thread.c.
 *
 * @param lock is the ticket lock.
 * @param state is the state which receives the next ticket of the lock.
 */
void TicketLockGetState(
  const SMP_ticket_lock_Control *lock,
  TicketLockState               *state
);

/**
 * @brief Waits for the count of acquire attempts since TicketLockGetState().
 *
 * The wait has no bound.
 *
 * @param state is the state of the ticket lock.
 * @param acquire_count is the count of acquire attempts.
 */
void TicketLockWaitForAcquires(
  const TicketLockState *state,
  unsigned int           acquire_count
);

/**
 * @brief Waits for the count of releases since TicketLockGetState().
 *
 * The wait has no bound.
 *
 * See TicketLockGetState() and tc-score-smp-thread.c.
 *
 * @param state is the state of the ticket lock.
 * @param release_count is the count of releases.
 */
void TicketLockWaitForReleases(
  const TicketLockState *state,
  unsigned int           release_count
);

/**
 * @brief Checks whether the ISR lock is available.
 *
 * @param lock is the ISR lock.
 *
 * @retval true No processor owns the lock.
 * @retval false A processor owns the lock.
 */
static inline bool ISRLockIsAvailable( const ISR_lock_Control *lock )
{
  return TicketLockIsAvailable( &lock->Lock.Ticket_lock );
}

/**
 * @brief Waits until a processor owns the ISR lock.
 *
 * The wait has no bound.
 *
 * @param lock is the ISR lock.
 */
static inline void ISRLockWaitForOwned( const ISR_lock_Control *lock )
{
  TicketLockWaitForOwned( &lock->Lock.Ticket_lock );
}

/**
 * @brief Waits until the count of processors waits for the ISR lock.
 *
 * The count excludes the owner of the lock.  The wait has no bound.
 *
 * Use this function to wait until the other processors spin on an ISR lock
 * which the test owns, for example the lock of a processor.  See
 * tc-sched-smp.c.
 *
 * @param lock is the ISR lock.
 * @param others is the count of processors which wait for the lock.
 */
static inline void ISRLockWaitForOthers(
  const ISR_lock_Control *lock,
  unsigned int            others
)
{
  TicketLockWaitForOthers( &lock->Lock.Ticket_lock, others );
}
#endif

/**
 * @brief Is the body of the idle thread of a test suite.
 *
 * Each test suite which configures this body defines the function.
 *
 * @param ignored is the argument of the idle thread body.
 *
 * @return The test suite defines the return value.
 */
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
