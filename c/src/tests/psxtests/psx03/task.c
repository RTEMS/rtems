/*  Task_1_through_3
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
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

#include "system.h"
#include <signal.h>

void *Task_1(
  void *argument
)
{
  int status;

  /* send SIGUSR2 to Init which is waiting on SIGUSR1 */

  print_current_time( "Task1: ", "" );
  status = pthread_kill( Init_id, SIGUSR2 );
  assert( !status );

  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */
}

void *Task_2(
  void *argument
)
{
  int status;

  /* send SIGUSR1 to Init which is waiting on SIGUSR1 */
 
  print_current_time( "Task2: ", "" );
  status = pthread_kill( Init_id, SIGUSR1 );
  assert( !status );
 
  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */
}
