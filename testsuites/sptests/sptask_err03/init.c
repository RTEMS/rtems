/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

const char rtems_test_name[] = "SP TASK ERROR 03";

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_name        task_name;
  rtems_status_code status;
  bool              skipUnsatisfied; 
  
  TEST_BEGIN();
  
  Task_name[ 1 ]       =  rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ]       =  rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ]       =  rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ]       =  rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 5 ]       =  rtems_build_name( 'T', 'A', '5', ' ' );
  Task_name[ 6 ]       =  rtems_build_name( 'T', 'A', '6', ' ' );
  Task_name[ 7 ]       =  rtems_build_name( 'T', 'A', '7', ' ' );
  Task_name[ 8 ]       =  rtems_build_name( 'T', 'A', '8', ' ' );
  Task_name[ 9 ]       =  rtems_build_name( 'T', 'A', '9', ' ' );
  Task_name[ 10 ]      =  rtems_build_name( 'T', 'A', 'A', ' ' );

  /* task create bad name */
  task_name = 1;
  status = rtems_task_create(
    0,
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_task_create with illegal name"
  );
  puts( "TA1 - rtems_task_create - RTEMS_INVALID_NAME" );

  /* null ID */
  status = rtems_task_create(
    Task_name[ 1 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    NULL
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ADDRESS,
    "rtems_task_create with NULL ID param"
  );
  puts( "TA1 - rtems_task_create - RTEMS_INVALID_ADDRESS" );

  /*
   * If the bsp provides its own stack allocator, then
   * skip the test that tries to allocate a stack that is too big.
   */

  skipUnsatisfied = false;
  if (rtems_configuration_get_stack_allocate_hook())
    skipUnsatisfied = true;

  if ( skipUnsatisfied ) {
    puts(
      "TA1 - rtems_task_create - stack size - RTEMS_UNSATISFIED  -- SKIPPED"
    );
  } else {
      status = rtems_task_create(
        task_name,
        1,
        rtems_configuration_get_work_space_size(),
        RTEMS_DEFAULT_MODES,
        RTEMS_DEFAULT_ATTRIBUTES,
        &Junk_id
      );
      fatal_directive_status(
        status,
        RTEMS_UNSATISFIED,
        "rtems_task_create with a stack size larger than the workspace"
      );
      puts( "TA1 - rtems_task_create - stack size - RTEMS_UNSATISFIED" );
  }
   status = rtems_task_create(
    Task_name[ 1 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE * 3,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_create(
    Task_name[ 2 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );
  puts( "TA1 - rtems_task_create - TA2 created - RTEMS_SUCCESSFUL" );

  status = rtems_task_suspend( Task_id[ 2 ] );
  directive_failed( status, "rtems_task_suspend of TA2" );
  puts( "TA1 - rtems_task_suspend - suspend TA2 - RTEMS_SUCCESSFUL" );

  status = rtems_task_suspend( Task_id[ 2 ] );
  fatal_directive_status(
    status,
    RTEMS_ALREADY_SUSPENDED,
    "rtems_task_suspend of suspended TA2"
  );
  puts( "TA1 - rtems_task_suspend - suspend TA2 - RTEMS_ALREADY_SUSPENDED" );

  status = rtems_task_resume( Task_id[ 2 ] );
  directive_failed( status, "rtems_task_resume of TA2" );
  puts( "TA1 - rtems_task_resume - TA2 resumed - RTEMS_SUCCESSFUL" );

  status = rtems_task_create(
    Task_name[ 3 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );
  puts( "TA1 - rtems_task_create - TA3 created - RTEMS_SUCCESSFUL" );

  status = rtems_task_create(
    Task_name[ 5 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 5 ]
  );
  directive_failed( status, "rtems_task_create of TA5" );
  puts( "TA1 - rtems_task_create - 5 created - RTEMS_SUCCESSFUL" );

  status = rtems_task_create(
    Task_name[ 6 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 6 ]
  );
  directive_failed( status, "rtems_task_create of TA6" );
  puts( "TA1 - rtems_task_create - 6 created - RTEMS_SUCCESSFUL" );

  status = rtems_task_create(
    Task_name[ 7 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 7 ]
  );
  directive_failed( status, "rtems_task_create of TA7" );
  puts( "TA1 - rtems_task_create - 7 created - RTEMS_SUCCESSFUL" );

  status = rtems_task_create(
    Task_name[ 8 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 8 ]
  );
  directive_failed( status, "rtems_task_create of TA8" );
  puts( "TA1 - rtems_task_create - 8 created - RTEMS_SUCCESSFUL" );

  status = rtems_task_create(
    Task_name[ 9 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 9 ]
  );
  directive_failed( status, "rtems_task_create of TA9" );
  puts( "TA1 - rtems_task_create - 9 created - RTEMS_SUCCESSFUL" );

  status = rtems_task_create(
    Task_name[ 10 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 10 ]
  );
  directive_failed( status, "rtems_task_create of TA10" );
  puts( "TA1 - rtems_task_create - 10 created - RTEMS_SUCCESSFUL" );

  status = rtems_task_create(
    task_name,
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_task_create for too many tasks"
  );
  puts( "TA1 - rtems_task_create - 11 - RTEMS_TOO_MANY" );

  TEST_END();
}
