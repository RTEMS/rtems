/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2022 Critical Software SA
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

#include "isvv_rtems_aux.h"

# define ITOA_STR_SIZE (8 * sizeof(int) + 1)

/* ############################################### */
/*                    Scheduler                    */
/* ############################################### */
rtems_id IdentifyScheduler( rtems_name name )
{
    rtems_id id;
    rtems_status_code sc;

    sc = rtems_scheduler_ident( name, &id );
    ASSERT_SUCCESS(sc);
    return id;
}

void SetScheduler(rtems_id task_id, rtems_id scheduler_id, rtems_task_priority priority) {
  rtems_status_code sc;

  sc = rtems_task_set_scheduler( task_id, scheduler_id, priority );
  ASSERT_SUCCESS( sc );
}

/* ############################################### */
/*                      Tasks                      */
/* ############################################### */
rtems_id DoCreateTask( rtems_task_config task_config )
{
    rtems_id id;
    rtems_status_code sc;

    sc = rtems_task_construct(&task_config, &id);
    ASSERT_SUCCESS(sc);
    return id;
}

void StartTask( rtems_id id, rtems_task_entry entry, void *arg )
{
    rtems_status_code sc;

    sc = rtems_task_start( id, entry, (rtems_task_argument) arg);
    ASSERT_SUCCESS( sc );
}

void DeleteTask( rtems_id id )
{
    if ( id != 0 ) {
	rtems_status_code sc;
	
	sc = rtems_task_delete( id );
	ASSERT_SUCCESS( sc );
    }
}

void SuspendTask( rtems_id id )
{
    rtems_status_code sc;

    sc = rtems_task_suspend( id );
    ASSERT_SUCCESS( sc );
}

void SuspendSelf( void )
{
    SuspendTask( RTEMS_SELF );
}

void ResumeTask( rtems_id id )
{
    rtems_status_code sc;

    sc = rtems_task_resume( id );
    ASSERT_SUCCESS( sc );
}

bool IsTaskSuspended( rtems_id id )
{
    rtems_status_code sc;

    sc = rtems_task_is_suspended( id );
    return sc == RTEMS_ALREADY_SUSPENDED;
}

rtems_id TaskSelfId(void)
{
    rtems_id id;
    rtems_status_code sc;

    sc = rtems_task_ident(RTEMS_WHO_AM_I, RTEMS_SEARCH_ALL_NODES, &id);
    ASSERT_SUCCESS( sc );
    return id;
}

rtems_task_priority SetTaskPriority(rtems_id id, rtems_task_priority prio) {
  rtems_status_code sc;
  rtems_task_priority old_prio;

  sc = rtems_task_set_priority(id, prio, &old_prio);
  ASSERT_SUCCESS(sc);
  return old_prio;
}

rtems_task_priority SetSelfPriority( rtems_task_priority priority )
{
  return SetTaskPriority( RTEMS_SELF, priority );
}

rtems_task_priority GetTaskPriority(rtems_id id) {
  return SetTaskPriority(id, RTEMS_CURRENT_PRIORITY);
}

void SetTaskMode(rtems_mode mode_set, rtems_mode mask, rtems_mode previous_mode_set) {
  rtems_status_code sc;

  sc = rtems_task_mode(mode_set, mask, &previous_mode_set);
  ASSERT_SUCCESS(sc);
}

rtems_mode GetTaskMode( void ) {
  rtems_status_code sc;
  rtems_mode mode_set;

  // First parameter is ignored when second parameter is set to RTEMS_CURRENT_MODE
  sc = rtems_task_mode(RTEMS_CURRENT_MODE, RTEMS_CURRENT_MODE, &mode_set);
  ASSERT_SUCCESS(sc);

  return mode_set;
}

// If task modes aren't equal to what we expect, print them out in hex
bool ASSERT_TASK_MODES_EQ(rtems_mode RESULT_MODE, rtems_mode EXPECTED_MODE)
{
  char str[ITOA_STR_SIZE];
  if (EXPECTED_MODE != RESULT_MODE)
  {
    print_string("TASK MODES NOT EXPECTED.\nRECEIVED-");
    print_string(itoa(RESULT_MODE, &str[0], 16));
    print_string(" EXPECTED-");
    print_string(itoa(EXPECTED_MODE, &str[0], 16));
    print_string("\n");
  }

  return (EXPECTED_MODE == RESULT_MODE);
}

/* ############################################### */
/*                   Semaphores                    */
/* ############################################### */
rtems_id CreateMutex( rtems_name name )
{
    rtems_status_code sc;
    rtems_id          id;

    id = INVALID_ID;
    sc = rtems_semaphore_create(name, 1,
                                RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
                                    RTEMS_INHERIT_PRIORITY,
                                0, &id);
    ASSERT_SUCCESS( sc );

    return id;
}

rtems_id CreateMutexNoProtocol( void )
{
    rtems_status_code sc;
    rtems_id          id;

    id = INVALID_ID;
    sc = rtems_semaphore_create(rtems_build_name('M', 'U', 'T', 'X'), 1,
				RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY, 0, &id);
    ASSERT_SUCCESS( sc );

  return id;
}

rtems_id CreateMutexNoLocking( rtems_name name )
{
    rtems_status_code sc;
    rtems_id          id;

    id = INVALID_ID;
    sc = rtems_semaphore_create(name, 3, RTEMS_NO_INHERIT_PRIORITY, 0, &id);
    ASSERT_SUCCESS( sc );

  return id;
}

rtems_id CreateMutexFIFO( void )
{
    rtems_status_code sc;
    rtems_id          id;
    
    id = INVALID_ID;
    sc = rtems_semaphore_create(rtems_build_name('M', 'U', 'T', 'X'), 1,
                                RTEMS_BINARY_SEMAPHORE | RTEMS_FIFO, 0, &id);
    ASSERT_SUCCESS(sc);

    return id;
}

rtems_id CreateMutexMrsP( rtems_name name )
{
    rtems_status_code sc;
    rtems_id          id;
    
    id = INVALID_ID;
    sc = rtems_semaphore_create(name, 1,
                                RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
				RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
                                PRIO_ULTRA_HIGH, &id);
    ASSERT_SUCCESS(sc);

    return id;
}

void ObtainMutex( rtems_id id )
{
    rtems_status_code sc;

    sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    ASSERT_SUCCESS(sc);
}

void ReleaseMutex( rtems_id id )
{
    rtems_status_code sc;

    sc = rtems_semaphore_release( id );
    ASSERT_SUCCESS( sc );
}

void DeleteMutex( rtems_id id )
{
    if ( id != INVALID_ID ) {
	rtems_status_code sc;
	
	sc = rtems_semaphore_delete( id );
	ASSERT_SUCCESS(sc);
    }
}

rtems_id CreateCounterSemaphore( rtems_name name, uint32_t count )
{
    rtems_status_code sc;
    rtems_id          id;

    id = INVALID_ID;
    sc = rtems_semaphore_create(name, count, RTEMS_DEFAULT_ATTRIBUTES, 0, &id);
    ASSERT_SUCCESS( sc );

    return id;
}

void ObtainCounterSemaphore( rtems_id id )
{
    ObtainMutex(id);
}

void ReleaseCounterSemaphore( rtems_id id)
{
    ReleaseMutex(id);
}


/* ############################################### */
/*                      Messages                   */
/* ############################################### */
rtems_id CreateMessageQueue(rtems_message_queue_config config) {
    rtems_id id;
    rtems_status_code sc;

    sc = rtems_message_queue_construct(&config, &id);
    ASSERT_SUCCESS(sc);
    return id;
}

void SendMessage(rtems_id id, void *buffer, size_t size) {
    rtems_status_code sc;

    sc = rtems_message_queue_send(id, buffer, size);
    ASSERT_SUCCESS(sc);
}

void ReceiveMessage(rtems_id id, void *buffer) {
    rtems_status_code sc;
    size_t size;
    
    sc = rtems_message_queue_receive(id, buffer, &size, RTEMS_DEFAULT_OPTIONS,
                                     RTEMS_NO_TIMEOUT);
    ASSERT_SUCCESS(sc);
}

void DeleteMessageQueue( rtems_id id )
{
    if ( id != INVALID_ID ) {
	rtems_status_code sc;
	
	sc = rtems_message_queue_delete( id );
	ASSERT_SUCCESS(sc);
    }
}


/* ############################################### */
/*                      Events                     */
/* ############################################### */
rtems_event_set ReceiveAllEvents( rtems_event_set events )
{
  rtems_status_code sc;
  rtems_event_set   received;

  received = 0;
  sc = rtems_event_receive(events, RTEMS_EVENT_ALL | RTEMS_WAIT,
                           RTEMS_NO_TIMEOUT, &received);
  ASSERT_SUCCESS(sc);
  return received;
}

rtems_event_set ReceiveAvailableEvents( void )
{
  rtems_status_code sc;
  rtems_event_set   events;

  events = 0;
  sc = rtems_event_receive(RTEMS_PENDING_EVENTS,
                           RTEMS_EVENT_ALL | RTEMS_WAIT, 0, &events);
  if (sc != RTEMS_TIMEOUT) ASSERT_SUCCESS(sc);

  return events;
}

rtems_event_set QueryPendingEvents( void )
{
  rtems_status_code sc;
  rtems_event_set   events;

  events = 0;
  sc = rtems_event_receive(RTEMS_PENDING_EVENTS,
                           RTEMS_EVENT_ALL | RTEMS_NO_WAIT, 0, &events);
  ASSERT_SUCCESS(sc);

  return events;
}

rtems_event_set ReceiveAnyEvents( void )
{
  return ReceiveAnyEventsTimed( RTEMS_NO_TIMEOUT );
}

rtems_event_set ReceiveAnyEventsTimed(rtems_interval ticks)
{
  rtems_status_code sc;
  rtems_event_set events;

  events = 0;
  sc = rtems_event_receive(RTEMS_ALL_EVENTS, RTEMS_EVENT_ANY | RTEMS_WAIT,
                           ticks, &events);
  ASSERT_SUCCESS(sc);

  return events;
}

void SendEvents( rtems_id id, rtems_event_set events )
{
  rtems_status_code sc;

  sc = rtems_event_send( id, events );
  ASSERT_SUCCESS(sc);
}

/* ############################################### */
/*                 Rate Monotonic                  */
/* ############################################### */
rtems_id CreateRateMonotonic( void )
{
    rtems_status_code sc;
    rtems_id          id;

    id = INVALID_ID;
    sc = rtems_rate_monotonic_create(rtems_build_name( 'R', 'M', 'O', 'N' ), &id);
    ASSERT_SUCCESS( sc );

    return id;
}

bool DoesPeriodTimeOut( rtems_id id, rtems_interval period )
{
  rtems_status_code sc;
  bool timeout = false;

  sc = rtems_rate_monotonic_period(id, period);
  if (sc == RTEMS_TIMEOUT) {
      timeout = true;
  } else {
      ASSERT_SUCCESS(sc);
  }

  return timeout;
}

void WaitPeriod( rtems_id id, rtems_interval period )
{
  rtems_status_code sc;

  sc = rtems_rate_monotonic_period(id, period);
  ASSERT_SUCCESS(sc);
}

void DeleteRateMonotonic( rtems_id id )
{
    if ( id != INVALID_ID ) {
	rtems_status_code sc;
	
	sc = rtems_rate_monotonic_delete( id );
	ASSERT_SUCCESS(sc);
    }
}


/* ############################################### */
/*                      Timer                      */
/* ############################################### */
rtems_id CreateTimer( rtems_name name )
{
    rtems_status_code sc;
    rtems_id          id;

    id = INVALID_ID;
    sc = rtems_timer_create(name, &id);
    ASSERT_SUCCESS( sc );

    return id;
}

void LaunchFunctionAfter(rtems_id id, rtems_interval ticks,
                         rtems_timer_service_routine_entry routine,
                         void *user_data) {
    rtems_status_code sc;
    sc = rtems_timer_fire_after(id, ticks, routine, user_data);
    ASSERT_SUCCESS(sc);
}

void ResetTimer( rtems_id id ) {
    rtems_status_code sc;
    sc = rtems_timer_reset( id );
    ASSERT_SUCCESS(sc);
}

void DeleteTimer( rtems_id id )
{
    if ( id != INVALID_ID ) {
	rtems_status_code sc;
	
	sc = rtems_timer_delete( id );
	ASSERT_SUCCESS(sc);
    }
}

/* ############################################### */
/*                     Barrier                     */
/* ############################################### */
rtems_id CreateAutomaticBarrier( uint8_t nbarriers )
{
    rtems_status_code sc;
    rtems_id          id;

    id = INVALID_ID;
    sc = rtems_barrier_create(rtems_build_name('B', 'A', 'R', 'A'),
                              RTEMS_BARRIER_AUTOMATIC_RELEASE, nbarriers, &id);
    ASSERT_SUCCESS( sc );

    return id;
}

rtems_id CreateManualBarrier(void)
{
    rtems_status_code sc;
    rtems_id          id;

    id = INVALID_ID;
    sc = rtems_barrier_create(rtems_build_name('B', 'A', 'R', 'M'),
                              RTEMS_BARRIER_MANUAL_RELEASE, 0, &id);
    ASSERT_SUCCESS( sc );

    return id;
}

void WaitAtBarrier( rtems_id id )
{
    rtems_status_code sc;

    sc = rtems_barrier_wait(id, BARRIER_TIMEOUT);
    ASSERT_SUCCESS( sc );
}

void ReleaseManualBarrier( rtems_id id, uint32_t n_barriers_to_release )
{
    rtems_status_code sc;
    uint32_t released;
    uint32_t total_released = 0;
    
    while (total_released < n_barriers_to_release) {
        sc = rtems_barrier_release(id, &released);
        ASSERT_SUCCESS(sc);
        total_released += released;
    }
}

void DeleteBarrier( rtems_id id )
{
    if ( id != INVALID_ID ) {
	rtems_status_code sc;
	
	sc = rtems_barrier_delete( id );
	ASSERT_SUCCESS(sc);
    }
}

/* ############################################### */
/*                      Other                      */
/* ############################################### */
/*     char GetRandomChar(void) { */
/*   return "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" */
/*       [rand() % 62]; */
/* } */

/* rtems_name BuildRandomName( void ) { */
/*     return rtems_build_name(GetRandomChar(),GetRandomChar(),GetRandomChar(),GetRandomChar()); */
/* } */

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

void WaitForExecutionStop( rtems_id task_id )
{
#if defined( RTEMS_SMP )
    Thread_Control *the_thread;

    the_thread = GetThread( task_id );

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
    
    intend_to_block = THREAD_WAIT_CLASS_OBJECT |
    THREAD_WAIT_STATE_INTEND_TO_BLOCK;

    while ( _Thread_Wait_flags_get_acquire( the_thread ) != intend_to_block ) {
	/* Wait */
    }
#else
    (void) task_id;
#endif
}

rtems_task_priority GetPriorityByScheduler(rtems_id task_id,
                                           rtems_id scheduler_id) {
    rtems_status_code   sc;
    rtems_task_priority priority;
    
    priority = PRIO_INVALID;
    sc = rtems_task_get_priority( task_id, scheduler_id, &priority );
    
    if ( sc != RTEMS_SUCCESSFUL ) {
	return PRIO_INVALID;
    }

    return priority;
}
