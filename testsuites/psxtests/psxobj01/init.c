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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <tmacros.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument ignored);

rtems_task Init(
  rtems_task_argument ignored
)
{
  Objects_Name_or_id_lookup_errors namerc;
  Objects_Information              TestClass;
  Objects_Id                       id;
  char                             name[64];
  bool                             bc;

  puts( "\n\n*** POSIX OBJECT TEST 1 ***" );

  /* very fake object class to test with */
  _Objects_Initialize_information(
    &TestClass,
    1,           /* the_api */
    4,           /* the_class */
    0,           /* maximum */
    4,           /* size */
    true,        /* is_string */
    10           /* maximum_name_length */
    #if defined(RTEMS_MULTIPROCESSING)
      ,
      false,       /* supports_global */
      NULL         /* Objects_Thread_queue_Extract_callout extract */
    #endif
  );

  puts( "INIT - _Objects_Name_to_id_string - NULL name" );
  namerc = _Objects_Name_to_id_string( &TestClass, NULL, &id );
  if ( namerc != OBJECTS_INVALID_NAME ) {
    printf( "ERROR - Status = %d\n", namerc );
    rtems_test_exit(0);
  }

  puts( "INIT - _Objects_Name_to_id_string - NULL ID" );
  namerc = _Objects_Name_to_id_string( &TestClass, name, NULL );
  if ( namerc != OBJECTS_INVALID_ADDRESS ) {
    printf( "ERROR - Status = %d\n", namerc );
    rtems_test_exit(0);
  }

  puts( "INIT - _Objects_Name_to_id_string - name of non-existent object" );
  strcpy( name, "NOT FOUND" );
  namerc = _Objects_Name_to_id_string( &TestClass, name, &id );
  if ( namerc != OBJECTS_INVALID_NAME ) {
    printf( "ERROR - Status = %d\n", namerc );
    rtems_test_exit(0);
  }

  /* out of memory error ONLY when POSIX is enabled */
  puts( "INIT - _Objects_Set_name fails - out of memory" );
  rtems_workspace_greedy_allocate( NULL, 0 );

  bc = _Objects_Set_name( &TestClass, &_Thread_Executing->Object, name );
  rtems_test_assert( bc == false );

  puts( "*** END OF POSIX OBJECT TEST 1 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS  1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
