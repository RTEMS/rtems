/*  Exit_test
 *
 *  This routine safely stops the test and prints some information
 *
 *  Input parameters:   NONE
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

void Exit_test( void )
{
  rtems_status_code status;
  rtems_mode        old_mode;

  /*
   * Wait a bit before shutting down so we don't screw up the other node
   * when our MPCI shuts down
   */
 
  rtems_task_wake_after(20);
 
  status = rtems_task_mode( RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &old_mode );
  directive_failed( status, "rtems_task_mode" );

  MPCI_Print_statistics();

  rtems_shutdown_executive( 0 );
}
