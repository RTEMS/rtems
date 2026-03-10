/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
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

#include "tmacros.h"

#include <rtems/libcsupport.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "test_iodev.h"

#define TEST_NAME_LENGTH 10

#define IODEV_PATH "/dev/iodev0"

const char rtems_test_name[] = "IODEV 1";

static void run_test( void );

static void assert_timing( struct timespec *start_time, int expected_secs )
{
  struct timespec end_time;
  struct timespec duration;

  clock_gettime( CLOCK_REALTIME, &end_time );
  _Timespec_Subtract( start_time, &end_time, &duration );
  rtems_test_assert( duration.tv_sec >= expected_secs - 1 );
  rtems_test_assert( duration.tv_sec <= expected_secs );
}

static void run_test( void )
{
  rtems_iodev               *iodev;
  rtems_resource_snapshot    snapshot;
  int                        status;
  FILE                      *file;
  int                        fd;
  size_t                     count;
  rtems_iodev_region         region;
  rtems_iodev_event_info     event;
  rtems_iodev_event_args     event_args;
  size_t                     i;
  void                      *map_addr;
  struct timespec            start_time;
  struct test_iodev_dev_info test_dev_info;

  /* Check resource usage on creation and deletion */
  rtems_resource_snapshot_take( &snapshot );

  iodev = test_iodev_init();
  rtems_test_assert( iodev != NULL );

  rtems_iodev_destroy_unregistered( iodev );
  iodev = NULL;

  rtems_test_assert( rtems_resource_snapshot_check( &snapshot ) );

  /* Check resource usage on registration */
  rtems_resource_snapshot_take( &snapshot );

  iodev = test_iodev_init();
  rtems_test_assert( iodev != NULL );

  status = rtems_iodev_register( iodev, IODEV_PATH );
  rtems_test_assert( !status );

  status = rtems_iodev_unregister_and_destroy( IODEV_PATH );
  rtems_test_assert( !status );

  rtems_test_assert( rtems_resource_snapshot_check( &snapshot ) );

  /* Initialize the iodev device driver and iodev */
  iodev = test_iodev_init();
  rtems_test_assert( iodev != NULL );

  /* Register the iodev as a device */
  status = rtems_iodev_register( iodev, IODEV_PATH );
  rtems_test_assert( !status );

  file = fopen( IODEV_PATH, "r+" );
  rtems_test_assert( file != NULL );

  fd = fileno( file );

  status = ioctl( fd, RTEMS_IODEV_IOCTL_DEVICE_INFO, &test_dev_info );
  rtems_test_assert( !status );
  rtems_test_assert( test_dev_info.id == TEST_IODEV_TEST_DEV_ID );

  /* Test valid regions */
  status = ioctl( fd, RTEMS_IODEV_IOCTL_REGION_COUNT, &count );
  rtems_test_assert( !status );
  rtems_test_assert( count == TEST_IODEV_REGION_COUNT );

  for ( i = 0; i < count; i++ ) {
    region.index = i;
    region.address = NULL;
    region.size = 0;
    region.name = NULL;
    status = ioctl( fd, RTEMS_IODEV_IOCTL_REGION_GET, &region );
    rtems_test_assert( !status );
    rtems_test_assert( region.size == TEST_IODEV_REGION_SIZE );
    rtems_test_assert( region.address != NULL );
    rtems_test_assert( region.name != NULL );

    map_addr = mmap(
      NULL,
      region.size,
      ( PROT_READ | PROT_WRITE ),
      MAP_SHARED,
      fd,
      region.index
    );
    rtems_test_assert( map_addr != MAP_FAILED );
    rtems_test_assert( map_addr == region.address );

    status = munmap( map_addr, region.size );
    rtems_test_assert( !status );
  }

  /* Test invalid region failure */
  map_addr = mmap(
    NULL,
    region.size,
    ( PROT_READ | PROT_WRITE ),
    MAP_SHARED,
    fd,
    TEST_IODEV_REGION_COUNT
  );
  rtems_test_assert( map_addr == MAP_FAILED );

  /* Test event info */
  status = ioctl( fd, RTEMS_IODEV_IOCTL_EVENT_COUNT, &count );
  rtems_test_assert( !status );
  rtems_test_assert( count == TEST_IODEV_EVENT_COUNT );

  for ( i = 0; i < count; i++ ) {
    event.index = i;
    status = ioctl( fd, RTEMS_IODEV_IOCTL_EVENT_INFO, &event );
    rtems_test_assert( !status );
    rtems_test_assert( event.name != NULL );
    rtems_test_assert( event.index == i );
  }

  /* Test TEST_IODEV_EVENT_SECS second event */
  event_args.index = 0;
  event_args.timeout.tv_sec = 0;
  event_args.timeout.tv_nsec = 0;
  event_args.args = NULL;

  clock_gettime( CLOCK_REALTIME, &start_time );

  status = ioctl( fd, RTEMS_IODEV_IOCTL_EVENT_WAIT, &event_args );

  rtems_test_assert( !status );
  assert_timing( &start_time, TEST_IODEV_EVENT_SECS );
  rtems_test_assert( event_args.timedout == false );

  /* Test large timeout */
  event_args.index = 0;
  event_args.timeout.tv_sec = 8;
  event_args.timeout.tv_nsec = 0;
  event_args.args = NULL;

  clock_gettime( CLOCK_REALTIME, &start_time );

  status = ioctl( fd, RTEMS_IODEV_IOCTL_EVENT_WAIT, &event_args );

  rtems_test_assert( !status );
  assert_timing( &start_time, TEST_IODEV_EVENT_SECS );
  rtems_test_assert( event_args.timedout == false );

  event_args.index = 1;
  event_args.timeout.tv_sec = 8;
  event_args.timeout.tv_nsec = 0;
  event_args.args = NULL;

  clock_gettime( CLOCK_REALTIME, &start_time );

  status = ioctl( fd, RTEMS_IODEV_IOCTL_EVENT_WAIT, &event_args );

  rtems_test_assert( !status );
  assert_timing( &start_time, 8 );
  rtems_test_assert( event_args.timedout == true );

  /* Test timeout feature */
  event_args.index = 0;
  event_args.timeout.tv_sec = 2;
  event_args.timeout.tv_nsec = 0;
  event_args.args = NULL;

  clock_gettime( CLOCK_REALTIME, &start_time );

  status = ioctl( fd, RTEMS_IODEV_IOCTL_EVENT_WAIT, &event_args );

  rtems_test_assert( !status );
  assert_timing( &start_time, 2 );
  rtems_test_assert( event_args.timedout == true );

  /* Test invalid event */
  event_args.index = TEST_IODEV_EVENT_COUNT;
  event_args.timeout.tv_sec = 2;
  event_args.timeout.tv_nsec = 0;
  event_args.args = NULL;

  status = ioctl( fd, RTEMS_IODEV_IOCTL_EVENT_WAIT, &event_args );
  rtems_test_assert( status );

  /* Test MAP_PRIVATE failure */
  map_addr =
    mmap( NULL, region.size, ( PROT_READ | PROT_WRITE ), MAP_PRIVATE, fd, 0 );
  rtems_test_assert( map_addr == MAP_FAILED );

  /* Test MAP_FIXED failure */
  map_addr =
    mmap( NULL, region.size, ( PROT_READ | PROT_WRITE ), MAP_FIXED, fd, 0 );
  rtems_test_assert( map_addr == MAP_FAILED );

  /* Test PROT_READ and PROT_WRITE requirement */
  map_addr = mmap( NULL, region.size, ( PROT_READ ), MAP_SHARED, fd, 0 );
  rtems_test_assert( map_addr == MAP_FAILED );

  map_addr = mmap( NULL, region.size, ( PROT_WRITE ), MAP_SHARED, fd, 0 );
  rtems_test_assert( map_addr == MAP_FAILED );

  map_addr = mmap( NULL, region.size, ( PROT_NONE ), MAP_SHARED, fd, 0 );
  rtems_test_assert( map_addr == MAP_FAILED );

  map_addr = mmap( NULL, region.size, ( PROT_EXEC ), MAP_SHARED, fd, 0 );
  rtems_test_assert( map_addr == MAP_FAILED );

  map_addr = mmap(
    NULL,
    region.size,
    ( PROT_READ | PROT_WRITE | PROT_EXEC ),
    MAP_SHARED,
    fd,
    0
  );
  rtems_test_assert( map_addr == MAP_FAILED );

  status = fclose( file );
  rtems_test_assert( !status );

  status = rtems_iodev_unregister_and_destroy( IODEV_PATH );
  rtems_test_assert( !status );
}

static void Init( rtems_task_argument arg )
{
  (void) arg;

  TEST_BEGIN();

  run_test();

  TEST_END();
  rtems_test_exit( 0 );
}

#define CONFIGURE_MICROSECONDS_PER_TICK 2000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 7

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
