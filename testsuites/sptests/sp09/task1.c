/*  Task_1
 *
 *  This task generates all possible errors for the RTEMS executive.
 *
 *  Input parameters:
 *    argument - task argument
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
  exit( 0 );
}
