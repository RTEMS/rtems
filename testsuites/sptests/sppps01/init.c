/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2022 German Aerospace Center (DLR)
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

#include <errno.h>
#include <rtems/test.h>
#include <rtems/test-info.h>
#include <rtems/timespec.h>
#include <rtems/rtems/event.h>
#define _KERNEL
#include <sys/timepps.h>

const char rtems_test_name[] = "SPPPS 1";

#define PPS_EVENT RTEMS_EVENT_0
#define TASK_WAITING RTEMS_EVENT_1
#define PPS_EVENT_RECEIVED RTEMS_EVENT_2

struct test_pps_device {
  struct pps_state pps;
  rtems_id task_waiting;
  int wokenup;
};

typedef struct {
  rtems_id main_task;
  struct test_pps_device *pps_dev;
} test_context;

T_TEST_CASE( WaitPPSEventDefaultHandler )
{
  int status;
  struct test_pps_device pps_dev;
  struct pps_fetch_args fetch;

  memset( &pps_dev, 0, sizeof( pps_dev ) );
  pps_dev.task_waiting = RTEMS_INVALID_ID;

  pps_dev.pps.ppscap = PPS_CAPTUREBOTH;
  pps_init_abi( &pps_dev.pps );
  pps_dev.pps.ppsparam.mode = PPS_CAPTUREASSERT;

  /* If no timeout is requested, pps_fetch() doesn't call the default handler */
  memset( &fetch, 0, sizeof( fetch ) );
  status = pps_ioctl( PPS_IOC_FETCH, (caddr_t)&fetch, &pps_dev.pps );
  T_eq_int( status, 0 );

  fetch.timeout.tv_sec = 1;
  status = pps_ioctl( PPS_IOC_FETCH, (caddr_t)&fetch, &pps_dev.pps );
  T_eq_int( status, ETIMEDOUT );
}

static void fake_wakeup(struct pps_state *pps)
{
  struct test_pps_device *pps_dev;

  pps_dev = RTEMS_CONTAINER_OF( pps, struct test_pps_device, pps );
  pps_dev->wokenup++;
}

T_TEST_CASE( PPSEventEarlyReturns )
{
  struct test_pps_device pps_dev;

  memset( &pps_dev, 0, sizeof( pps_dev ) );
  pps_dev.task_waiting = RTEMS_INVALID_ID;

  pps_dev.pps.ppscap = PPS_CAPTUREBOTH;
  pps_init_abi( &pps_dev.pps );
  pps_dev.pps.wakeup = fake_wakeup;
  pps_dev.pps.ppsparam.mode = PPS_CAPTUREASSERT;

  pps_capture( &pps_dev.pps );
  /* Trigger event with a different event than the one previously selected */
  pps_event( &pps_dev.pps, PPS_CAPTURECLEAR );
  T_eq_int( pps_dev.wokenup, 0 );

  pps_dev.pps.ppsparam.mode = PPS_CAPTURECLEAR;

  /* Wait th_generation to be updated */
  rtems_task_wake_after( 2 );
  pps_event( &pps_dev.pps, PPS_CAPTURECLEAR );
  T_eq_int( pps_dev.wokenup, 0 );

  /* Save current timecounter in pps_state object */
  pps_capture( &pps_dev.pps );
  pps_event( &pps_dev.pps, PPS_CAPTURECLEAR );
  T_eq_int( pps_dev.wokenup, 0 );

  pps_capture( &pps_dev.pps );
  pps_event( &pps_dev.pps, PPS_CAPTURECLEAR );
  T_eq_int( pps_dev.wokenup, 1 );
}

static void wakeup(struct pps_state *pps)
{
  struct test_pps_device *pps_dev;

  pps_dev = RTEMS_CONTAINER_OF( pps, struct test_pps_device, pps );
  if (pps_dev->task_waiting != RTEMS_INVALID_ID)
    rtems_event_send( pps_dev->task_waiting, PPS_EVENT );
}

static int wait(struct pps_state *pps, struct timespec timeout)
{
  rtems_status_code sc;
  rtems_event_set out;
  uint32_t timeoutticks;
  struct test_pps_device *pps_dev;

  pps_dev = RTEMS_CONTAINER_OF( pps, struct test_pps_device, pps );
  pps_dev->task_waiting = rtems_task_self();

  timeoutticks = rtems_timespec_to_ticks(&timeout);
  sc = rtems_event_receive( PPS_EVENT, RTEMS_DEFAULT_OPTIONS, timeoutticks, &out );
  return rtems_status_code_to_errno(sc);
}

static void pps_task(rtems_task_argument arg)
{
  int status;
  rtems_status_code sc;
  struct pps_fetch_args fetch;
  test_context *ctx;

  ctx = (test_context *) arg;

  fetch.tsformat = PPS_TSFMT_TSPEC;
  fetch.timeout.tv_sec = 1;
  fetch.timeout.tv_nsec = 0;

  sc = rtems_event_send( ctx->main_task, TASK_WAITING );
  T_rsc_success( sc );
  status = pps_ioctl( PPS_IOC_FETCH, (caddr_t)&fetch, &ctx->pps_dev->pps );
  T_eq_int( status, 0 );
  sc = rtems_event_send( ctx->main_task, PPS_EVENT_RECEIVED );
  T_rsc_success( sc );

  rtems_task_exit();
}

T_TEST_CASE( WakeupTaskWithPPSEvent )
{
  int status;
  rtems_status_code sc;
  struct test_pps_device pps_dev;
  struct pps_kcbind_args kcbind;
  test_context ctx;
  rtems_id pps_task_id;
  rtems_task_priority pps_task_prio = 1;
  rtems_event_set out;

  memset( &pps_dev, 0, sizeof( pps_dev ) );
  pps_dev.task_waiting = RTEMS_INVALID_ID;
  ctx.pps_dev = &pps_dev;
  ctx.main_task = rtems_task_self();

  pps_dev.pps.ppscap = PPS_CAPTUREBOTH;
  pps_init_abi( &pps_dev.pps );
  pps_dev.pps.wait = wait;
  pps_dev.pps.wakeup = wakeup;
  pps_dev.pps.ppsparam.mode = PPS_CAPTUREASSERT;

  kcbind.kernel_consumer = PPS_KC_HARDPPS;
  kcbind.edge = PPS_CAPTUREASSERT;
  kcbind.tsformat = PPS_TSFMT_TSPEC;
  status = pps_ioctl( PPS_IOC_KCBIND, (caddr_t)&kcbind, &pps_dev.pps );
  T_eq_int( status, 0 );

  /* Save current timecounter in pps_state object */
  pps_capture( &pps_dev.pps );
  pps_event( &pps_dev.pps, PPS_CAPTUREASSERT );

  sc = rtems_task_create(
    rtems_build_name('P', 'P', 'S', 'E'),
    pps_task_prio,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &pps_task_id
  );
  T_rsc_success( sc );
  sc = rtems_task_start( pps_task_id, pps_task, (rtems_task_argument) &ctx );
  T_rsc_success( sc );

  sc = rtems_event_receive( TASK_WAITING, RTEMS_DEFAULT_OPTIONS, RTEMS_MILLISECONDS_TO_TICKS(100), &out );
  T_rsc_success( sc );

  /* Capture event and send wake-up */
  pps_capture( &pps_dev.pps );
  pps_event( &pps_dev.pps, PPS_CAPTUREASSERT );

  sc = rtems_event_receive( PPS_EVENT_RECEIVED, RTEMS_DEFAULT_OPTIONS, RTEMS_MILLISECONDS_TO_TICKS(100), &out );
  T_rsc_success( sc );
}

static rtems_task Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
