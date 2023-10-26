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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

#include "tr-event-mgr-model.h"

static const char PromelaModelEventsMgr[] = "/PML-EventsMgr";

#define INPUT_EVENTS ( RTEMS_EVENT_5 | RTEMS_EVENT_23 )

rtems_id CreateWakeupSema( void )
{
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_semaphore_create(
    rtems_build_name( 'W', 'K', 'U', 'P' ),
    0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &id
  );
  T_assert_rsc_success( sc );

  return id;
}

void DeleteWakeupSema( rtems_id id )
{
  if ( id != 0 ) {
    rtems_status_code sc;

    sc = rtems_semaphore_delete( id );
    T_rsc_success( sc );
  }
}

void Wait( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain( id, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  T_quiet_rsc_success( sc );
}

void Wakeup( rtems_id id )
{
  rtems_status_code sc;

  sc = rtems_semaphore_release( id );
  T_quiet_rsc_success( sc );
}

rtems_event_set GetPending( Context *ctx )
{
  rtems_event_set pending;
  rtems_status_code sc;

  sc = ( *ctx->receive )(
    RTEMS_PENDING_EVENTS,
    RTEMS_DEFAULT_OPTIONS,
    0,
    &pending
  );
  T_quiet_rsc_success( sc );

  return pending;
}


rtems_option mergeopts( bool wait, bool wantall )
{
  rtems_option opts;

  if ( wait ) { opts = RTEMS_WAIT; }
  else { opts = RTEMS_NO_WAIT; } ;
  if ( wantall ) { opts |= RTEMS_EVENT_ALL; }
  else { opts |= RTEMS_EVENT_ANY; } ;
  return opts;
}


/*
 * Here we need a mapping from model "task numbers/names" to thread Id's here
 *  Promela Process 3 corresponds to Task 0 (Worker), doing Send
 *  Promela Process 4 corresponds to Task 1 (Runner), doing Receive
 */
rtems_id mapid( Context *ctx, int pid )
{
  rtems_id mapped_id;

  switch ( pid ) {
    case 1 : mapped_id = ctx->worker_id ; break;
    case 2 : mapped_id = ctx->runner_id; break;
    default : mapped_id = 0xffffffff; break;
  }
  return mapped_id;
}

void checkTaskIs( rtems_id expected_id )
{
  rtems_id own_id;

  own_id = _Thread_Get_executing()->Object.id;
  T_eq_u32( own_id, expected_id );
}

void initialise_pending( rtems_event_set pending[], int max )
{
  int i;

  for( i=0; i < max; i++ ) {
    pending[i] = 0;
  }
}

void initialise_semaphore( Context *ctx, rtems_id semaphore[] )
{
  semaphore[0] = ctx->worker_wakeup;
  semaphore[1] = ctx->runner_wakeup;
}

void ShowWorkerSemaId( Context *ctx ) {
  T_printf( "L:ctx->worker_wakeup = %d\n", ctx->worker_wakeup );
}

void ShowRunnerSemaId( Context *ctx ) {
  T_printf( "L:ctx->runner_wakeup = %d\n", ctx->runner_wakeup );
}

static void RtemsModelEventsMgr_Teardown(
  RtemsModelEventsMgr_Context *ctx
)
{
  rtems_status_code   sc;
  rtems_task_priority prio;

  T_log( T_NORMAL, "Runner Teardown" );

  prio = 0;
  sc = rtems_task_set_priority( RTEMS_SELF, PRIO_DEFAULT, &prio );
  T_rsc_success( sc );
  T_eq_u32( prio, PRIO_NORMAL );

  if ( ctx->worker_id != 0 ) {
    T_printf( "L:Deleting Task id : %d\n", ctx->worker_id );
    sc = rtems_task_delete( ctx->worker_id );
    T_rsc_success( sc );
  }

  T_log( T_NORMAL, "Deleting Worker Wakeup Semaphore" );
  DeleteWakeupSema( ctx->worker_wakeup );
  T_log( T_NORMAL, "Deleting Runner Wakeup Semaphore" );
  DeleteWakeupSema( ctx->runner_wakeup );
}

void RtemsModelEventsMgr_Teardown_Wrap( void *arg )
{
  RtemsModelEventsMgr_Context *ctx;

  ctx = arg;
  RtemsModelEventsMgr_Teardown( ctx );
}


size_t RtemsModelEventsMgr_Scope( void *arg, char *buf, size_t n )
{
  RtemsModelEventsMgr_Context *ctx;
  size_t m;
  int p10;
  int tnum ;
  char digit;

  ctx = arg;
  p10 = POWER_OF_10;

  m = T_str_copy(buf, PromelaModelEventsMgr, n);
  buf += m;
  tnum = ctx->this_test_number;
  while( p10 > 0 && m < n )
  {
    digit = (char) ( (int) '0' + tnum / p10 );
    buf[0] = digit;
    ++buf;
    ++m;
    tnum = tnum % p10;
    p10 /= 10;
  }
  return m;
}

void RtemsModelEventsMgr_Cleanup(
  RtemsModelEventsMgr_Context *ctx
)
{
  rtems_status_code sc;
  rtems_event_set events;

  events = 0;
  sc = ( *ctx->receive )(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    0,
    &events
  );
  if ( sc == RTEMS_SUCCESSFUL ) {
    T_quiet_ne_u32( events, 0 );
  } else {
    T_quiet_rsc( sc, RTEMS_UNSATISFIED );
    T_quiet_eq_u32( events, 0 );
  }
}
