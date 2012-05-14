/*  Task_1
 *
 *  This task generates all possible errors for the RTEMS executive.
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Task_1(
  rtems_task_argument argument
)
{
  Screen1();
  rtems_test_pause_and_screen_number( 2 );

  Screen2();
  rtems_test_pause_and_screen_number( 3 );

  Screen3();
  rtems_test_pause_and_screen_number( 4 );

  Screen4();
  rtems_test_pause_and_screen_number( 5 );

  Screen5();
  rtems_test_pause_and_screen_number( 6 );

  Screen6();
  rtems_test_pause_and_screen_number( 7 );

  Screen7();
  rtems_test_pause_and_screen_number( 8 );

  Screen8();
  rtems_test_pause_and_screen_number( 9 );

  Screen9();
  rtems_test_pause_and_screen_number( 10 );

  Screen10();
  rtems_test_pause_and_screen_number( 11 );

  Screen11();
  rtems_test_pause_and_screen_number( 12 );

  Screen12();
  rtems_test_pause_and_screen_number( 13 );

  Screen13();
  rtems_test_pause_and_screen_number( 14 );

  Screen14();

  puts( "*** END OF TEST 9 ***" );
  rtems_test_exit( 0 );
}
