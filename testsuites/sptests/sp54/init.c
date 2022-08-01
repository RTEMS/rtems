/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @@file
 *
 *  Odd Id Cases where API configured but No Threads
 *    + Possibly Valid Id passed to directive
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/sysinit.h>
#include <rtems/score/memory.h>
#include <rtems/score/thread.h>

#include <tmacros.h>

const char rtems_test_name[] = "SP 54";

static void *Init( uintptr_t ignored )
{
  rtems_status_code                    status;
  rtems_task_priority                  pri;
  rtems_id                             id;
  const rtems_api_configuration_table *config;

  puts( "Init - use valid id of API class with no objects" );
  status = rtems_task_set_priority(
    rtems_build_id(0x2,0x1,0x01,0x0001) /* 0xa010001 */,
    RTEMS_CURRENT_PRIORITY,
    &pri
  );
  fatal_directive_status( status, RTEMS_INVALID_ID, "rtems_task_set_priority" );

  puts( "Init - lookup name within API class with no objects" );
  status = rtems_task_ident(
    rtems_build_id( 0, 0, 0x12, 0x3456) /* 0x123456 */,
    RTEMS_SEARCH_ALL_NODES,
    &id
  );
  fatal_directive_status( status, RTEMS_INVALID_NAME, "rtems_task_ident" );

  rtems_test_assert( rtems_configuration_get_do_zero_of_workspace() );

  config = rtems_configuration_get_rtems_api_configuration();
  rtems_test_assert( config->number_of_initialization_tasks == 0 );
  rtems_test_assert( config->User_initialization_tasks_table == NULL );

  TEST_END();
  rtems_test_exit(0);
}

static void check_dirty_memory( void )
{
  unsigned char *p;

  TEST_BEGIN();

  p = _Memory_Allocate( _Memory_Get(), sizeof( *p ), RTEMS_ALIGNOF( *p ) );
  rtems_test_assert( p != NULL );
  rtems_test_assert( *p == 0xcf );

  p = (unsigned char *) _Thread_Information.Objects.initial_objects;
  rtems_test_assert( *p == 0xcf );
}

RTEMS_SYSINIT_ITEM(
  check_dirty_memory,
  RTEMS_SYSINIT_DIRTY_MEMORY,
  RTEMS_SYSINIT_ORDER_LAST
);

static void check_zero_workspace_automatically( void )
{
  unsigned char *p;

  p = _Memory_Allocate( _Memory_Get(), sizeof( *p ), RTEMS_ALIGNOF( *p ) );
  rtems_test_assert( p != NULL );
  rtems_test_assert( *p == 0 );

  p = (unsigned char *) _Thread_Information.Objects.initial_objects;
  rtems_test_assert( *p == 0 );
}

RTEMS_SYSINIT_ITEM(
  check_zero_workspace_automatically,
  RTEMS_SYSINIT_ZERO_MEMORY,
  RTEMS_SYSINIT_ORDER_LAST
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

/*
 *  In this application, the initialization task performs the system
 *  initialization and then transforms itself into the idle task.
 */
#define CONFIGURE_IDLE_TASK_BODY Init
#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

#define CONFIGURE_DIRTY_MEMORY

/*
 *  Ensure we test the case where memory is zero.
 */
#define CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
