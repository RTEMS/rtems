/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define CONFIGURE_INIT
#include "system.h"

#include <iostream>

#include <stdlib.h>

const char rtems_test_name[] = "IOSTREAM";

rtems_task Init(
  rtems_task_argument ignored
)
{
  std::cout << std::endl << std::endl
    << "*** BEGIN OF TEST " << rtems_test_name << " ***" << std::endl;
  std::cout << "Hello World" << std::endl;
  std::cout << "*** END OF TEST " << rtems_test_name << " ***" << std::endl;
  exit( 0 );
}

