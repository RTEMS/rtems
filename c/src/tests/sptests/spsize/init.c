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

#include "apps.h"
#undef EXTERN
#define EXTERN
#include "conftbl.h"
#include "gvar.h"

rtems_task Test_task();
void size_rtems( int mode );

rtems_task Init(
  rtems_task_argument argument
)
{
  int choice;

  setvbuf(stdout, 0, _IONBF, 0);

  do {
    puts( "\n*** RTEMS SIZE PROGRAM ***" );
    printf( "\n\nPlease select program mode:\n" );
    printf( "  1) Print Formulas\n" );
    printf( "  2) Determine Workspace Size\n" );
    printf( "  3) Exit\n" );
    printf( "Enter number of choice (1,2,3) : " );

    choice = getint();
    switch( choice ) {
      case 1: size_rtems( 1 );  break;
      case 2: size_rtems( 0 );  break;
      case 3: exit( 0 );
      default:  continue;
    }
  } while ( FOREVER );
}
