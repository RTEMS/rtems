/* 
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <sched.h>

void *POSIX_Init(
  void *argument
)
{
  int             status;
  int             value;
  int             i;
  mqd_t           mqs[CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES];
  mqd_t           mq2;
  mqd_t           *n_mq1;
  mqd_t           *n_mq2;
  struct timespec waittime;
  char            failure_msg[80];


  puts( "\n\n*** POSIX MESSAGE QUEUE TEST ***" );

#if 0
  /* Modes are currently unsupported */

  /*
   * Validate all mq_open return paths.
   */

  puts( "Init: mq_open - mq1 SUCCESSFUL" );
  n_mq1 = mq_open( "mq1", O_CREAT, 00777, 1 );
  assert( n_mq1 != SEM_FAILED );

  puts( "Init: mq_open - Create an Existing mq (EEXIST)" );
  n_mq2 = mq_open("mq1", O_CREAT | O_EXCL, 00777, 1);
  fatal_directive_status( 
    (int) n_mq2, (int ) SEM_FAILED, "mq_open error return status" );
  fatal_directive_status( errno, EEXIST,  "mq_open errno EEXIST");

  puts( "Init: mq_open - Open new mq without create flag (ENOENT)" );
  n_mq2 = mq_open("mq3", O_EXCL, 00777, 1);
  fatal_directive_status( 
    (int) n_mq2, (int ) SEM_FAILED, "mq_open error return status" );
  fatal_directive_status( errno, ENOENT,  "mq_open errno EEXIST"); 

  /*
   * XXX - Could not hit the following errors:
   *   E_POSIX_Semaphore_Create_support only fails if
   *     ENOSYS - When semaphore is shared between processes.
   *     ENOSPC - When out of memory.
   */

  /*
   * Validate we can wait on a message queue opened with mq_open.
   */

  puts( "Init: mq_wait on mq1" );
  status = mq_receive(n_mq1);
  fatal_directive_status( status, 0, "mq_wait opened message queue"); 

  /*
   * Validate a second open returns the same message queue.
   */

  puts( "Init: mq_open - Open an existing mq ( same id )" );
  n_mq2 = mq_open("mq1", 0 );
  fatal_directive_status( 
    (int) n_mq2, (int ) n_mq1, "mq_open error return status" );
  
  /*
   * Unlink the message queue, then verify an open of the same name produces a 
   * different message queue.
   */

  puts( "Init: mq_unlink - mq1 SUCCESSFUL" );
  status = mq_unlink( "mq1" );
  fatal_directive_status( status, 0, "mq_unlink locked message queue"); 

  puts( "Init: mq_open - Reopen mq1 SUCCESSFUL with a different id" );
  n_mq2 = mq_open( "mq1", O_CREAT | O_EXCL, 00777, 1);
  assert( n_mq2 != SEM_FAILED );
  assert( n_mq2 != n_mq1 );

  /*
   * Validate we can call close on a message queue opened with mq_open.
   */

  puts( "Init: mq_close (1) - SUCCESSFUL" );
  status = mq_close( n_mq1 );
  fatal_directive_status( status, 0, "mq_close message queue"); 


  /*
   * Validate it n_mq2 (the last open for mq1 name can be
   * correctly closed and unlinked.
   */

  puts( "Init: mq_close (2) - SUCCESSFUL" );
  status = mq_close( n_mq2 );
  fatal_directive_status( status, 0, "mq_close message queue"); 

  puts( "Init: mq_unlink - mq1 (2) SUCCESSFUL" );
  status = mq_unlink( "mq1" );
  fatal_directive_status( status, 0, "mq_unlink locked message queue"); 

  puts( "Init: mq_close - UNSUCCESSFUL (EINVAL)" );
  status = mq_close(n_mq2);
  fatal_directive_status( status, -1, "mq_close error return status");
  fatal_directive_status( errno, EINVAL, "mq_close errno EINVAL");

  puts( "Init: mq_unlink - UNSUCCESSFUL (ENOENT)" );
  status = mq_unlink("mq1");
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, ENOENT, "mq_close errno EINVAL");


  /*
   * Validate we can unlink (2)
   */

  puts( "Init: mq_unlink (NULL) - EINVAL" );
  status = mq_unlink( NULL );
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, EINVAL, "mq_unlink errno value"); 

  puts( "Init: mq_unlink (\"\") - EINVAL" );
  status = mq_unlink( "" );
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, EINVAL, "mq_unlink errno value"); 

  /*
   * XXX - Cant' create location OBJECTS_ERROR or OBJECTS_REMOTE.
   *       mq_close and mq_unlink.
   */

  puts( "Init: mq_unlink - UNSUCCESSFUL (ENOENT)" );
  status = mq_unlink("mq2");
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, ENOENT, "mq_unlink errno ENOENT");
  assert( (status == -1) && (errno == ENOENT) );

#endif
  puts( "*** END OF POSIX MESSAGE QUEUE TEST ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
