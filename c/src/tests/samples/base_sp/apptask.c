/*  Application_task
 *
 *  This routine is as an example of an application task which
 *  prints a message including its RTEMS task id.  This task
 *  then invokes exit to return to the monitor.
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
#include "libcsupport.h"

rtems_task Application_task(
  rtems_task_argument argument
)
{
  rtems_id          tid;
  rtems_status_code status;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );

  printf( "Application task was invoked with argument (%d) "
          "and has id of 0x%x\n", argument, tid );

  printf( "*** END OF SAMPLE SINGLE PROCESSOR APPLICATION ***\n" );
  exit( 0 );
}
