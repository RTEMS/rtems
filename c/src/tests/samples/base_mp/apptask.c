/*  Application_task
 *
 *  This routine is as an example of an application task which
 *  prints a message including its RTEMS task id.  This task
 *  then invokes exit to return to the monitor.
 *
 *  Input parameters:
 *    node - processor's node number
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
 *  apptask.c,v 1.2 1995/05/31 17:05:14 joel Exp
 */

#include "system.h"

rtems_task Application_task(
  rtems_task_argument node
)
{
  rtems_id          tid;
  rtems_status_code status;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  printf( "This task was invoked with the node argument (%d)\n", node );
  printf( "This task has the id of 0x%x\n",  tid );
  printf( "*** END OF SAMPLE MULTIPROCESSOR APPLICATION ***\n" );
  exit( 0 );
}
