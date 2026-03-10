/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2026 Aaron Nyholm
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

#include <stdlib.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/seterr.h>
#include <rtems/timespec.h>

#include "test_iodev.h"

#define TIMED_RETURN_SEC TEST_IODEV_EVENT_SECS

typedef struct test_iodev {
  rtems_iodev       *iodev;
  rtems_iodev_region regions[ TEST_IODEV_REGION_COUNT ];
  rtems_id           server_id;
  rtems_id           client_id;
  rtems_mutex        lock;
  bool               waiting;
} test_iodev;

static test_iodev tiodev;

int test_iodev_get_device_info( rtems_iodev *iodev, void *event_info );

int test_iodev_get_event_info(
  rtems_iodev                   *iodev,
  struct rtems_iodev_event_info *event_info
);

int test_iodev_event_wait(
  rtems_iodev                   *iodev,
  struct rtems_iodev_event_args *event_args
);

int test_iodev_get_regions( rtems_iodev *iodev, rtems_iodev_region **regions );

int test_iodev_get_region_count( rtems_iodev *iodev, size_t *region_count );

int test_iodev_get_event_count( rtems_iodev *iodev, size_t *event_count );

void test_iodev_priv_destroy( rtems_iodev *iodev );

static void test_iodev_server()
{
  rtems_status_code sc;
  rtems_event_set   event_in;

  for ( ;; ) {
    sc = rtems_event_receive(
      RTEMS_EVENT_0,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &event_in
    );
    if ( sc != RTEMS_SUCCESSFUL ) {
      continue;
    }

    rtems_task_wake_after(
      RTEMS_MILLISECONDS_TO_TICKS( 1000 * TIMED_RETURN_SEC )
    );

    rtems_mutex_lock( &tiodev.lock );
    if ( tiodev.waiting ) {
      rtems_event_send( tiodev.client_id, RTEMS_EVENT_0 );
    }
    rtems_mutex_unlock( &tiodev.lock );
  }

  rtems_task_exit();
}

int test_iodev_get_device_info( rtems_iodev *iodev, void *event_info )
{
  (void) iodev;
  struct test_iodev_dev_info *info;

  info = event_info;
  info->id = TEST_IODEV_TEST_DEV_ID;

  return 0;
}

int test_iodev_get_event_info(
  rtems_iodev                   *iodev,
  struct rtems_iodev_event_info *event_info
)
{
  (void) iodev;

  if ( event_info->index == 0 ) {
    event_info->name = "timed_return";
    event_info->args = NULL;
  } else if ( event_info->index == 1 ) {
    event_info->name = "noreturn";
    event_info->args = NULL;
  } else {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }
  return 0;
}

int test_iodev_event_wait(
  rtems_iodev                   *iodev,
  struct rtems_iodev_event_args *event_args
)
{
  (void) iodev;

  rtems_status_code sc;
  rtems_event_set   event_in;
  rtems_interval    timeout;
  rtems_event_set   event_out;

  if ( event_args->index == 0 ) {
    event_in = RTEMS_EVENT_0;
    rtems_event_send( tiodev.server_id, RTEMS_EVENT_0 );
  } else if ( event_args->index == 1 ) {
    event_in = RTEMS_EVENT_1;
  } else {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( event_args->timeout.tv_sec == 0 && event_args->timeout.tv_nsec == 0 ) {
    timeout = RTEMS_NO_TIMEOUT;
  } else {
    timeout = rtems_timespec_to_ticks( &event_args->timeout );
  }

  rtems_mutex_lock( &tiodev.lock );
  tiodev.client_id = rtems_task_self();
  tiodev.waiting = true;
  rtems_mutex_unlock( &tiodev.lock );

  sc = rtems_event_receive( event_in, RTEMS_WAIT, timeout, &event_out );

  rtems_mutex_lock( &tiodev.lock );
  tiodev.waiting = false;
  rtems_mutex_unlock( &tiodev.lock );

  event_args->timedout = ( sc == RTEMS_TIMEOUT );

  return 0;
}

int test_iodev_get_regions( rtems_iodev *iodev, rtems_iodev_region **regions )
{
  (void) iodev;

  *regions = tiodev.regions;

  return 0;
}

int test_iodev_get_region_count( rtems_iodev *iodev, size_t *region_count )
{
  (void) iodev;

  *region_count = TEST_IODEV_REGION_COUNT;
  return 0;
}

int test_iodev_get_event_count( rtems_iodev *iodev, size_t *event_count )
{
  (void) iodev;

  *event_count = TEST_IODEV_EVENT_COUNT;
  return 0;
}

void test_iodev_priv_destroy( rtems_iodev *iodev )
{
  (void) iodev;
  int i;

  for ( i = 0; i < TEST_IODEV_REGION_COUNT; i++ ) {
    free( tiodev.regions[ i ].address );
  }

  rtems_task_delete( tiodev.server_id );
  rtems_mutex_destroy( &tiodev.lock );
}

rtems_iodev *test_iodev_init( void )
{
  rtems_status_code sc;

  tiodev.iodev = rtems_iodev_alloc_and_init( sizeof( rtems_iodev ) );

  if ( tiodev.iodev == NULL ) {
    return NULL;
  }

  rtems_mutex_init( &tiodev.lock, "test_iodev" );
  rtems_mutex_lock( &tiodev.lock );
  tiodev.waiting = false;
  rtems_mutex_unlock( &tiodev.lock );

  tiodev.regions[ 0 ].index = 0;
  tiodev.regions[ 0 ].address = malloc( TEST_IODEV_REGION_SIZE );
  if ( tiodev.regions[ 0 ].address == NULL ) {
    rtems_iodev_destroy_unregistered( tiodev.iodev );
    return NULL;
  }
  tiodev.regions[ 0 ].size = TEST_IODEV_REGION_SIZE;
  tiodev.regions[ 0 ].name = "iodev_region_zero";

  tiodev.regions[ 1 ].index = 1;
  tiodev.regions[ 1 ].address = malloc( TEST_IODEV_REGION_SIZE );
  if ( tiodev.regions[ 1 ].address == NULL ) {
    free( tiodev.regions[ 0 ].address );
    rtems_iodev_destroy_unregistered( tiodev.iodev );
    return NULL;
  }
  tiodev.regions[ 1 ].size = TEST_IODEV_REGION_SIZE;
  tiodev.regions[ 1 ].name = "iodev_region_one";

  sc = rtems_task_create(
    rtems_build_name( 'T', 'I', 'O', 'S' ),
    100,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &tiodev.server_id
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    free( tiodev.regions[ 0 ].address );
    free( tiodev.regions[ 1 ].address );
    rtems_iodev_destroy_unregistered( tiodev.iodev );
    return NULL;
  }

  sc = rtems_task_start(
    tiodev.server_id,
    (rtems_task_entry) test_iodev_server,
    (rtems_task_argument) NULL
  );

  tiodev.iodev->get_device_info = test_iodev_get_device_info;
  tiodev.iodev->event_wait = test_iodev_event_wait;
  tiodev.iodev->get_regions = test_iodev_get_regions;
  tiodev.iodev->get_region_count = test_iodev_get_region_count;
  tiodev.iodev->get_event_count = test_iodev_get_event_count;
  tiodev.iodev->get_event_info = test_iodev_get_event_info;
  tiodev.iodev->priv_destroy = test_iodev_priv_destroy;

  return tiodev.iodev;
}
