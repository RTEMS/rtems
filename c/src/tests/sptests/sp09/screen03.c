/*  Screen3
 *
 *  This routine generates error screen 3 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include "system.h"

extern rtems_configuration_table BSP_Configuration;
extern rtems_cpu_table           _CPU_Table;

void Screen3()
{
  rtems_name        task_name;
  rtems_status_code status;

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

  /*
   * If the bsp provides its own stack allocator, then
   * skip the test that tries to allocate a stack that is too big.
   */

  if (_CPU_Table.stack_allocate_hook)
  {
      puts( "TA1 - rtems_task_create - stack size - RTEMS_UNSATISFIED  -- SKIPPED" );
  }
  else
  {
      status = rtems_task_create(
        task_name,
        1,
        BSP_Configuration.work_space_size,
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
  puts( "TA1 - rtems_task_create - RTEMS_MP_NOT_CONFIGURED" );
}
