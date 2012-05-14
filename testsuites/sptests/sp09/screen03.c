/*  Screen3
 *
 *  This routine generates error screen 3 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

void Screen3()
{
  rtems_name        task_name;
  rtems_status_code status;
  bool              skipUnsatisfied;

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
   *
   * If on the m32c, we can't even ask for enough memory to trip this
   * error.
   */

  skipUnsatisfied = false;
  if (rtems_configuration_get_stack_allocate_hook())
    skipUnsatisfied = true;
  #if defined(__m32c__)
    skipUnsatisfied = true;
  #endif

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
    Task_name[ 4 ],
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 4 ]
  );
  directive_failed( status, "rtems_task_create of TA4" );
  puts( "TA1 - rtems_task_create - 4 created - RTEMS_SUCCESSFUL" );

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

  /*
   *  The check for an object being global is only made if
   *  multiprocessing is enabled.
   */

#if defined(RTEMS_MULTIPROCESSING)
  status = rtems_task_create(
    task_name,
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_GLOBAL,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_MP_NOT_CONFIGURED,
    "rtems_task_create of global task in a single cpu system"
  );
#endif
  puts( "TA1 - rtems_task_create - RTEMS_MP_NOT_CONFIGURED" );
}
