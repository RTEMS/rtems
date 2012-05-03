/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
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

#define CONFIGURE_INIT
#include "system.h"

/* #define HAVE_MENU */

void size_rtems( int mode );

rtems_task Init(
  rtems_task_argument argument
)
{
#if defined(HAVE_MENU)
  int choice = 0;
#endif

  setvbuf(stdout, 0, _IONBF, 0);

  puts( "\n*** RTEMS SIZE PROGRAM ***" );
  size_rtems( 1 );
  puts( "*** END OF RTEMS SIZE PROGRAM ***" );
  rtems_test_exit( 0 );
#if defined(HAVE_MENU)
  do {
    printf( "\n\nPlease select program mode:\n" );
    printf( "  1) Print Formulas\n" );
    printf( "  2) Determine Workspace Size\n" );
    printf( "  3) Exit\n" );
    printf( "Enter number of choice (1,2,3) : " );

    choice = getint();
    switch( choice ) {
      case 1: size_rtems( 1 );  break;
      case 2: size_rtems( 0 );  break;
      case 3: rtems_test_exit( 0 );
      default:  continue;
    }
  } while ( FOREVER );
#endif
}
