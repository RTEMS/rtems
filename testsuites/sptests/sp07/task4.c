/*  Task_4
 *
 *  This routine serves as a low priority test task that should exit
 *  a soon as it runs to test the taskexitted user extension.
 *  execute.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Task_4(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_id id;

  assert_extension_counts( &Task_created[ 0 ], 0x2 | 0x4 | 0x8 | 0x10 );
  assert_extension_counts( &Task_started[ 0 ], 0x2 | 0x4 | 0x8 | 0x10 );
  assert_extension_counts( &Task_restarted[ 0 ], 0x4 );
  assert_extension_counts( &Task_deleted[ 0 ], 0x0 );

  /* Kill the zombies */
  status = rtems_task_create(
    rtems_build_name( 'L', 'A', 'Z', 'Y' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );

  assert_extension_counts( &Task_deleted[ 0 ], 0x2 | 0x4 | 0x8 );

  puts( "TA4 - exitting task" );
}
