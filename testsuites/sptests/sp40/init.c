/*
 *  IO Manager Dynamic Registration
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define TEST_INIT
#include "system.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  puts( "\n\n*** TEST 40 ***" );

  /*
   *  Register two drivers to fill up the table
   */

  /* add major = 4 */

  /* add major = 3 */

  /*
   *  Unregister major = 3 to leave slot which is not at
   *  the end of the Driver Table.
   */

  puts( "*** END OF TEST 40 ***" );
  rtems_test_exit( 0 );
}
