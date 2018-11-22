/**
 *  @file
 *  Odd Object Name/Id Error Cases
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

#include <rtems/score/objectimpl.h>

const char rtems_test_name[] = "PSXOBJ 1";

typedef struct {
  Objects_Control Object;
} Test_Control;

/* very fake object class to test with */
OBJECTS_INFORMATION_DEFINE( Test, 1, 4, Test_Control, 0, 10, NULL );

static rtems_task Init(
  rtems_task_argument ignored
)
{
  Objects_Get_by_name_error  error;
  Objects_Control           *the_object;
  char                       name[64];
  size_t                     name_len;
  bool                       bc;

  TEST_BEGIN();

  puts( "INIT - _Objects_Get_by_name - NULL name" );
  _Objects_Allocator_lock();
  the_object = _Objects_Get_by_name( &Test_Information, NULL, NULL, &error );
  _Objects_Allocator_unlock();
  rtems_test_assert( the_object == NULL );
  rtems_test_assert( error == OBJECTS_GET_BY_NAME_INVALID_NAME );

  puts( "INIT - _Objects_Get_by_name - name too long" );
  strcpy( name, "TOOOOOOOOOOOOOOOOOO LONG" );
  _Objects_Allocator_lock();
  the_object = _Objects_Get_by_name( &Test_Information, name, NULL, &error );
  _Objects_Allocator_unlock();
  rtems_test_assert( the_object == NULL );
  rtems_test_assert( error == OBJECTS_GET_BY_NAME_NAME_TOO_LONG );

  puts( "INIT - _Objects_Get_by_name - name of non-existent object" );
  strcpy( name, "NOT FOUND" );
  name_len = 123;
  _Objects_Allocator_lock();
  the_object = _Objects_Get_by_name( &Test_Information, name, &name_len, &error );
  _Objects_Allocator_unlock();
  rtems_test_assert( the_object == NULL );
  rtems_test_assert( error == OBJECTS_GET_BY_NAME_NO_OBJECT );
  rtems_test_assert( name_len == 9 );

  /* out of memory error ONLY when POSIX is enabled */
  puts( "INIT - _Objects_Set_name fails - out of memory" );
  rtems_workspace_greedy_allocate( NULL, 0 );

  bc = _Objects_Set_name(
    &Test_Information,
    &_Thread_Get_executing()->Object,
    name
  );
  rtems_test_assert( bc == false );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS  1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
