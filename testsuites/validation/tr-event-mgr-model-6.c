/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TestsuitesModel0
 *
 * @brief This source file contains test cases related to a formal model.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
 *                    Trinity College Dublin (http://www.tcd.ie)
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

/*
 * This file was automatically generated.  Do not edit it manually.
 * Please have a look at
 *
 * https://docs.rtems.org/branches/master/eng/req/howto.html
 *
 * for information how to maintain and re-generate this file.
 */

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>


#include "tr-event-mgr-model.h"

//  ===============================================

// @@@ 0 NAME Event_Manager_TestGen
// @@@ 0 DEF NO_OF_EVENTS 4
#define NO_OF_EVENTS 4
// @@@ 0 DEF EVTS_NONE 0
#define EVTS_NONE 0
// @@@ 0 DEF EVTS_PENDING 0
#define EVTS_PENDING 0
// @@@ 0 DEF EVT_0 1
#define EVT_0 1
// @@@ 0 DEF EVT_1 2
#define EVT_1 2
// @@@ 0 DEF EVT_2 4
#define EVT_2 4
// @@@ 0 DEF EVT_3 8
#define EVT_3 8
// @@@ 0 DEF NO_TIMEOUT 0
#define NO_TIMEOUT 0
// @@@ 0 DEF TASK_MAX 3
#define TASK_MAX 3
// @@@ 0 DEF BAD_ID 3
#define BAD_ID 3
// @@@ 0 DEF SEMA_MAX 2
#define SEMA_MAX 2
// @@@ 0 DEF RC_OK RTEMS_SUCCESSFUL
#define RC_OK RTEMS_SUCCESSFUL
// @@@ 0 DEF RC_InvId RTEMS_INVALID_ID
#define RC_InvId RTEMS_INVALID_ID
// @@@ 0 DEF RC_InvAddr RTEMS_INVALID_ADDRESS
#define RC_InvAddr RTEMS_INVALID_ADDRESS
// @@@ 0 DEF RC_Unsat RTEMS_UNSATISFIED
#define RC_Unsat RTEMS_UNSATISFIED
// @@@ 0 DEF RC_Timeout RTEMS_TIMEOUT
#define RC_Timeout RTEMS_TIMEOUT
// @@@ 0 DECL byte sendrc 0
static rtems_status_code sendrc = 0;
// @@@ 0 DECL byte recrc 0
static rtems_status_code recrc = 0;
// @@@ 0 DCLARRAY EvtSet pending TASK_MAX
static rtems_event_set pending[TASK_MAX];
// @@@ 0 DCLARRAY byte recout TASK_MAX
static rtems_event_set recout[TASK_MAX];
// @@@ 0 DCLARRAY Semaphore semaphore SEMA_MAX
static rtems_id semaphore[SEMA_MAX];

//  ===== TEST CODE SEGMENT 0 =====

static void TestSegment0( Context* ctx ) {
  /* Test Name is defined in the Test Case code (tc-model-events-mgr.c) */
  
  T_log(T_NORMAL,"@@@ 0 INIT");
  initialise_pending( pending, TASK_MAX );
  initialise_semaphore( ctx, semaphore );
  
}

//  ===== TEST CODE SEGMENT 3 =====

static void TestSegment3( Context* ctx ) {
  T_log(T_NORMAL,"@@@ 3 TASK Worker");
  checkTaskIs( ctx->worker_id );
  
  T_log(T_NORMAL,"@@@ 3 CALL LowerPriority");
  SetSelfPriority( PRIO_LOW );
  rtems_task_priority prio;
  rtems_status_code sc;
  sc = rtems_task_set_priority( RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &prio );
  T_rsc_success( sc );
  T_eq_u32( prio, PRIO_LOW );
  
  T_log(T_NORMAL,"@@@ 3 WAIT 0");
  Wait( semaphore[0] );
  
  T_log(T_NORMAL,"@@@ 3 CALL StartLog");
  T_thread_switch_log *log;
  log = T_thread_switch_record_4( &ctx->thread_switch_log );
  
  T_log(T_NORMAL,"@@@ 3 CALL event_send 1 2 10 sendrc");
  T_log( T_NORMAL, "Calling Send(%d,%d)", mapid( ctx, 2), 10 );
  sendrc = ( *ctx->send )( mapid( ctx, 2 ), 10 );
  T_log( T_NORMAL, "Returned 0x%x from Send", sendrc );
  
  T_log(T_NORMAL,"@@@ 3 CALL CheckPreemption");
  log = (T_thread_switch_log *) &ctx->thread_switch_log;
  T_eq_sz( log->header.recorded, 2 );
  T_eq_u32( log->events[ 0 ].heir, ctx->runner_id );
  T_eq_u32( log->events[ 1 ].heir, ctx->worker_id );
  
  T_log(T_NORMAL,"@@@ 3 STATE 2 Ready");
  /* We (Task 2) must have been recently ready because we are running */

  T_log(T_NORMAL,"@@@ 3 STATE 1 OtherWait");
  /* Code to check that Task 1 is waiting (after pre-emption) */
  T_log(T_NORMAL,"@@@ 3 STATE 1 Ready");
  /* We (Task 1) must have been recently ready because we are running */

  T_log(T_NORMAL,"@@@ 3 SCALAR sendrc 0");
  T_rsc( sendrc, 0 );
  T_log(T_NORMAL,"@@@ 3 SIGNAL 1");
  Wakeup( semaphore[1] );
  
  T_log(T_NORMAL,"@@@ 3 STATE 1 Zombie");
  /* Code to check that Task 1 has terminated */
}

//  ===== TEST CODE SEGMENT 4 =====

static void TestSegment4( Context* ctx ) {
  T_log(T_NORMAL,"@@@ 4 TASK Runner");
  checkTaskIs( ctx->runner_id );
  
  T_log(T_NORMAL,"@@@ 4 SIGNAL 1");
  Wakeup( semaphore[1] );
  
  T_log(T_NORMAL,"@@@ 4 WAIT 1");
  Wait( semaphore[1] );
  
  T_log(T_NORMAL,"@@@ 4 SIGNAL 0");
  Wakeup( semaphore[0] );
  
  T_log(T_NORMAL,"@@@ 4 SCALAR pending 2 0");
  pending[2] = GetPending( ctx );
  T_eq_int( pending[2], 0 );
  
  T_log(T_NORMAL,"@@@ 4 CALL event_receive 10 1 1 0 2 recrc");
  T_log( T_NORMAL, "Calling Receive(%d,%d,%d,%d)", 10, mergeopts( 1, 1 ) ,0 , 2 ? &recout[2] : NULL );
  recrc = ( *ctx->receive )( 10, mergeopts( 1, 1 ), 0, 2 ? &recout[2] : NULL );
  T_log( T_NORMAL, "Returned 0x%x from Receive", recrc );
  
  T_log(T_NORMAL,"@@@ 4 STATE 2 EventWait");
  /* Code to check that Task 2 is waiting on events */
  T_log(T_NORMAL,"@@@ 4 STATE 2 Ready");
  /* We (Task 2) must have been recently ready because we are running */

  T_log(T_NORMAL,"@@@ 4 SCALAR recrc 0");
  T_rsc( recrc, 0 );
  T_log(T_NORMAL,"@@@ 4 SCALAR recout 2 10");
  T_eq_int( recout[2], 10 );
  T_log(T_NORMAL,"@@@ 4 SCALAR pending 2 0");
  pending[2] = GetPending( ctx );
  T_eq_int( pending[2], 0 );
  
  T_log(T_NORMAL,"@@@ 4 SIGNAL 0");
  Wakeup( semaphore[0] );
  
  T_log(T_NORMAL,"@@@ 4 STATE 2 Zombie");
  /* Code to check that Task 2 has terminated */
}

//  ===============================================



static void Runner( RtemsModelEventsMgr_Context *ctx )
{
  T_log( T_NORMAL, "Runner running" );
  TestSegment4( ctx );
  T_log( T_NORMAL, "Runner finished" );
}

static void Worker6( rtems_task_argument arg )
{
  Context *ctx;
  rtems_event_set events;

  ctx = (Context *) arg;

  T_log( T_NORMAL, "Worker Running" );
  TestSegment3( ctx );
  T_log( T_NORMAL, "Worker finished" );

  // (void) rtems_task_suspend( RTEMS_SELF );
  // Ensure we hold no semaphores
  Wakeup( ctx->worker_wakeup );
  Wakeup( ctx->runner_wakeup );
  // Wait for events so we don't terminate
  rtems_event_receive( RTEMS_ALL_EVENTS, RTEMS_DEFAULT_OPTIONS, 0, &events );

}

RTEMS_ALIGNED( RTEMS_TASK_STORAGE_ALIGNMENT ) static char WorkerStorage6[
  RTEMS_TASK_STORAGE_SIZE(
    MAX_TLS_SIZE + TEST_MINIMUM_STACK_SIZE,
    WORKER_ATTRIBUTES
  )
];

static const rtems_task_config WorkerConfig6 = {
  .name = rtems_build_name( 'W', 'O', 'R', 'K' ),
  .initial_priority = PRIO_LOW,
  .storage_area = WorkerStorage6,
  .storage_size = sizeof( WorkerStorage6 ),
  .maximum_thread_local_storage_size = MAX_TLS_SIZE,
  .initial_modes = RTEMS_DEFAULT_MODES,
  .attributes = WORKER_ATTRIBUTES
};


static void RtemsModelEventsMgr_Setup6(
  RtemsModelEventsMgr_Context *ctx
)
{
  rtems_status_code   sc;
  rtems_task_priority prio;

  T_log( T_NORMAL, "Runner Setup" );

  memset( ctx, 0, sizeof( *ctx ) );
  ctx->runner_thread = _Thread_Get_executing();
  ctx->runner_id = ctx->runner_thread->Object.id;

  T_log( T_NORMAL, "Creating Worker Wakeup Semaphore" );
  ctx->worker_wakeup = CreateWakeupSema();
  T_log( T_NORMAL, "Creating Runner Wakeup Semaphore" );
  ctx->runner_wakeup = CreateWakeupSema();

  sc = rtems_task_get_scheduler( RTEMS_SELF, &ctx->runner_sched );
  T_rsc_success( sc );

  #if defined(RTEMS_SMP)
  sc = rtems_scheduler_ident_by_processor( 1, &ctx->other_sched );
  T_rsc_success( sc );
  T_ne_u32( ctx->runner_sched, ctx->other_sched );
  #endif

  prio = 0;
  sc = rtems_task_set_priority( RTEMS_SELF, PRIO_NORMAL, &prio );
  T_rsc_success( sc );
  T_eq_u32( prio, PRIO_DEFAULT );

  sc = rtems_task_construct( &WorkerConfig6, &ctx->worker_id );
  T_log( T_NORMAL, "Construct Worker, sc = %x", sc );
  T_assert_rsc_success( sc );

  T_log( T_NORMAL, "Starting Worker..." );
  sc = rtems_task_start( ctx->worker_id, Worker6, (rtems_task_argument) ctx );
  T_log( T_NORMAL, "Started Worker, sc = %x", sc );
  T_assert_rsc_success( sc );
}


static void RtemsModelEventsMgr_Setup_Wrap6( void *arg )
{
  RtemsModelEventsMgr_Context *ctx;

  ctx = arg;
  RtemsModelEventsMgr_Setup6( ctx );
}


static RtemsModelEventsMgr_Context RtemsModelEventsMgr_Instance6;

static T_fixture RtemsModelEventsMgr_Fixture6 = {
  .setup = RtemsModelEventsMgr_Setup_Wrap6,
  .stop = NULL,
  .teardown = RtemsModelEventsMgr_Teardown_Wrap,
  .scope = RtemsModelEventsMgr_Scope,
  .initial_context = &RtemsModelEventsMgr_Instance6
};

static T_fixture_node RtemsModelEventsMgr_Node6;

void RtemsModelEventsMgr_Run6(
  rtems_status_code ( *send )( rtems_id, rtems_event_set ),
  rtems_status_code ( *receive )( rtems_event_set, rtems_option, rtems_interval, rtems_event_set * ),
  rtems_event_set (   *get_pending_events )( Thread_Control * ),
  unsigned int         wait_class,
  int                  waiting_for_event
)
{
  RtemsModelEventsMgr_Context *ctx;

  T_set_verbosity( T_NORMAL );

  T_log( T_NORMAL, "Runner Invoked" );
  T_log( T_NORMAL, "Runner Wait Class: %d", wait_class );
  T_log( T_NORMAL, "Runner WaitForEvent: %d", waiting_for_event );

  T_log( T_NORMAL, "Pushing Test Fixture..." );


  ctx = T_push_fixture(
    &RtemsModelEventsMgr_Node6,
    &RtemsModelEventsMgr_Fixture6
  );
  // This runs RtemsModelEventsMgr_Fixture

  T_log( T_NORMAL, "Test Fixture Pushed" );


  ctx->send = send;
  ctx->receive = receive;
  ctx->get_pending_events = get_pending_events;
  ctx->wait_class = wait_class;
  ctx->waiting_for_event = waiting_for_event;

  ctx->this_test_number = 6;

  // RtemsModelEventsMgr_Prepare( ctx );
  ctx->events_to_send = 0;
  ctx->send_status = RTEMS_INCORRECT_STATE;
  ctx->received_events = 0xffffffff;
  ctx->receive_option_set = 0;
  ctx->receive_timeout = RTEMS_NO_TIMEOUT;
  ctx->unsatisfied_pending = 0xffffffff;
  memset( &ctx->thread_switch_log, 0, sizeof( ctx->thread_switch_log ) );
  T_eq_u32( GetPending( ctx ), 0 );
  _Thread_Wait_flags_set( ctx->runner_thread, THREAD_WAIT_STATE_READY );

  TestSegment0( ctx );

  Runner( ctx );

  RtemsModelEventsMgr_Cleanup( ctx );

  T_log( T_NORMAL, "Run Pop Fixture" );
  ShowWorkerSemaId( ctx );
  T_pop_fixture();
  ShowWorkerSemaId( ctx );
}

/** @} */
