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
 *  http://www.rtems.com/license/LICENSE.
 */

#define CONFIGURE_INIT
#include "system.h"

#if BSP_SMALL_MEMORY
#include <stdio.h>
#else
#include <iostream>
#endif

#include <stdlib.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
#if BSP_SMALL_MEMORY
  printf ("NO STDC++. MEMORY TOO SMALL");
#else
  std::cout << "\n\n*** HELLO WORLD TEST ***" << std::endl;
  std::cout << "Hello World" << std::endl;
  std::cout << "*** END OF HELLO WORLD TEST ***" << std::endl;
#endif
  exit( 0 );
}

