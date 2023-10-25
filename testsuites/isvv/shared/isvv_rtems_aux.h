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

#include <rtems.h>
#include <rtems/score/percpu.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <rtems/score/threadimpl.h>
#pragma GCC diagnostic pop
#include "utils.h"

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

#define INVALID_ID 0xfffffffd
#define PRIO_INVALID 0xfffffffe

#define BARRIER_TIMEOUT 10000

#define SCHEDULER_A_NAME rtems_build_name( 'A', ' ', ' ', ' ' )
#define SCHEDULER_B_NAME rtems_build_name( 'B', ' ', ' ', ' ' )
#define SCHEDULER_C_NAME rtems_build_name( 'C', ' ', ' ', ' ' )
#define SCHEDULER_D_NAME rtems_build_name( 'D', ' ', ' ', ' ' )

typedef struct {
    rtems_name      name;
    uint32_t        count;
    size_t          max_message_size;
} message_config;

rtems_id IdentifyScheduler(rtems_name name);

void SetScheduler(rtems_id task_id, rtems_id scheduler_id, rtems_task_priority priority);

rtems_id DoCreateTask( rtems_task_config task_config );

void StartTask( rtems_id id, rtems_task_entry entry, void *arg );

void DeleteTask(rtems_id id);

void SuspendTask( rtems_id id );

void SuspendSelf( void );

void ResumeTask( rtems_id id );

bool IsTaskSuspended(rtems_id id);

rtems_id TaskSelfId(void);

rtems_task_priority SetTaskPriority( rtems_id id, rtems_task_priority prio );

rtems_task_priority SetSelfPriority( rtems_task_priority priority );

rtems_task_priority GetTaskPriority(rtems_id id);

void SetTaskMode(rtems_mode mode_set, rtems_mode mask, rtems_mode previous_mode_set);

rtems_mode GetTaskMode( void );

bool ASSERT_TASK_MODES_EQ(rtems_mode RESULT_MODE, rtems_mode EXPECTED_MODE);

rtems_id CreateMutex( rtems_name name );

rtems_id CreateMutexNoProtocol( void );

rtems_id CreateMutexNoLocking( rtems_name name );

rtems_id CreateMutexFIFO( void );

rtems_id CreateMutexMrsP( rtems_name name );

void ObtainMutex( rtems_id id );

void ReleaseMutex( rtems_id id );

void DeleteMutex( rtems_id id );

rtems_id CreateCounterSemaphore( rtems_name name, uint32_t count );

void ObtainCounterSemaphore( rtems_id id );

void ReleaseCounterSemaphore( rtems_id id);

rtems_id CreateMessageQueue(rtems_message_queue_config config);

void SendMessage(rtems_id id, void *buffer, size_t size);

void ReceiveMessage(rtems_id id, void *buffer);

void DeleteMessageQueue(rtems_id id);

rtems_event_set ReceiveAllEvents( rtems_event_set events );

rtems_event_set ReceiveAnyEvents(void);

rtems_event_set ReceiveAnyEventsTimed(rtems_interval ticks);

rtems_event_set ReceiveAvailableEvents( void );

rtems_event_set QueryPendingEvents( void );

void SendEvents( rtems_id id, rtems_event_set events );

rtems_id CreateRateMonotonic( void );

bool DoesPeriodTimeOut( rtems_id id, rtems_interval period );

void WaitPeriod( rtems_id id, rtems_interval period );

void DeleteRateMonotonic( rtems_id id );

rtems_id CreateTimer( rtems_name name );

void LaunchFunctionAfter(rtems_id id, rtems_interval ticks,
                         rtems_timer_service_routine_entry routine,
                         void *user_data);

void ResetTimer( rtems_id id );

void DeleteTimer( rtems_id id );

rtems_id CreateAutomaticBarrier( uint8_t nbarriers );

rtems_id CreateManualBarrier(void);

void WaitAtBarrier( rtems_id id );

void ReleaseManualBarrier( rtems_id id, uint32_t n_barriers_to_release);

void DeleteBarrier( rtems_id id );

char GetRandomChar( void );

rtems_name BuildRandomName( void );

Thread_Control *GetThread( rtems_id id );

void WaitForExecutionStop( rtems_id task_id );

void WaitForIntendToBlock( rtems_id task_id );

rtems_task_priority GetPriorityByScheduler( rtems_id task_id, rtems_id scheduler_id );
