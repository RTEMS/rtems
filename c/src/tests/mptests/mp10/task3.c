/*  Test_task3
 *
 *  This task attempts to receive control of a global semaphore.
 *  It should never receive control of the semaphore.
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

rtems_task Test_task3( restart )
rtems_task_argument restart;
{
  rtems_status_code status;

  if ( restart == 1 ) {
    status = rtems_task_delete( RTEMS_SELF );
    directive_failed( status, "rtems_task_delete" );
  }

  puts( "Getting SMID of semaphore" );

  do {
    status = rtems_semaphore_ident(
      Semaphore_name[ 1 ],
      RTEMS_SEARCH_ALL_NODES,
      &Semaphore_id[ 1 ]
    );
  } while ( !rtems_is_status_successful( status ) );

  puts( "Attempting to acquire semaphore ..." );
  status = rtems_semaphore_obtain(
    Semaphore_id[ 1 ],
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );
  directive_failed( status, "rtems_semaphore_obtain" );
}
