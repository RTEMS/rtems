/*  Get_all_counters
 *
 *  This routine allows TA5 to atomically obtain the iteration counters.
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
 *  getall.c,v 1.3 1995/12/19 20:21:07 joel Exp
 */

#include "system.h"

void Get_all_counters()
{
  rtems_mode        previous_mode;
  rtems_status_code status;

  status = rtems_task_mode(
    RTEMS_NO_PREEMPT,
    RTEMS_PREEMPT_MASK,
    &previous_mode
  );
  directive_failed( status, "rtems_task_mode to RTEMS_NO_PREEMPT" );

  Temporary_count = Count;
  Count.count[ 1 ] = 0;
  Count.count[ 2 ] = 0;
  Count.count[ 3 ] = 0;
  Count.count[ 4 ] = 0;
  Count.count[ 5 ] = 0;

  status = rtems_task_mode( RTEMS_PREEMPT, RTEMS_PREEMPT_MASK, &previous_mode );
  directive_failed( status, "rtems_task_mode to RTEMS_PREEMPT" );
}
