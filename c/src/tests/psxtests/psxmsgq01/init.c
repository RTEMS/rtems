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
#include <fcntl.h>
#include <time.h>
#include <tmacros.h>

char Queue_Name[PATH_MAX + 2];
char *Get_Queue_Name(
  int i
)
{
  sprintf(Queue_Name,"mq%d",i+1);
  return Queue_Name;
}

char *Get_Too_Long_Name()
{
  int i;

  for ( i=0; i< PATH_MAX+1; i++ )
    Queue_Name[i] = 'N';
  Queue_Name[i] = '\0';
  return Queue_Name;
}

/*
 * Opens CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES then leaves size queues
 * opened but closes the rest.
 */

void validate_mq_open_error_codes(
  mqd_t   *mqs,      /* Must be large enough for Maximum to be opened. */
  int      size
)
{
  int             i;
  mqd_t           n_mq2;
  struct mq_attr  attr;
  int             status;

  assert( size < (CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES-1) );

  /*
   * Validate mq_open errors that can occur when no queues are open.
   *  EINVAL
   *  ENOENT
   *  EINTR
   */

  /*
   * XXX EINVAL - inappropriate name was given for the message queue
   */

  attr.mq_maxmsg = -1;
  puts( "mq_open - Create with maxmsg (-1) (EINVAL)" );
  n_mq2 = mq_open("mq2", O_CREAT, O_RDONLY, &attr);
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, EINVAL,  "mq_open errno EINVAL"); 

  attr.mq_msgsize = -1;
  puts( "mq_open - Create with msgsize (-1) (EINVAL)" );
  n_mq2 = mq_open("mq2", O_CREAT, O_RDONLY, &attr);
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, EINVAL,  "mq_open errno EINVAL"); 


  puts( "mq_open - Open new mq without create flag (ENOENT)" );
  n_mq2 = mq_open("mq3", O_EXCL, O_RDONLY, NULL);
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, ENOENT,  "mq_open errno ENOENT"); 

  /*
   * XXX EINTR  - call was interrupted by a signal
   */

  /*
   * XXX ENAMETOOLONG - Not checked in either sem_open or mq_open is
   *                    this an error? 
   */

  puts( "mq_open - Open with too long of a name (ENAMETOOLONG)" );
  n_mq2 = mq_open( Get_Too_Long_Name(), O_CREAT, O_RDONLY, NULL );
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, ENAMETOOLONG,  "mq_open errno ENAMETOOLONG"); 
  
  /*
   * Open maximum number of message queues
   */

  puts( "mq_open - SUCCESSFUL" );
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES; i++) {
    mqs[i] = mq_open( Get_Queue_Name(i), O_CREAT, O_RDONLY, NULL );
    assert( mqs[i] != (-1) );
    /*XXX - Isn't there a more general check */ 
  }

  /*
   * Validate open errors that must occur after message queues are open.
   *   EACCES
   *   EEXIST
   *   EMFILE
   *   ENFILE
   */

  /*
   * XXX EACCES - permission to create is denied.
   */

  /*
   * XXX EACCES - queue exists permissions specified by o_flag are denied.
  puts( "mq_open - open mq as write (EACCES)" );
  n_mq2 = mq_open("mq1", O_CREAT, O_WRONLY, NULL);
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, EACCES,  "mq_open errno EACCES");
   */

  puts( "mq_open - Create an Existing mq (EEXIST)" );
  n_mq2 = mq_open("mq1", O_CREAT | O_EXCL, O_RDONLY, NULL);
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, EEXIST,  "mq_open errno EEXIST");


  /*
   * XXX EMFILE  - Too many message queues open
   */

  puts( "mq_open - system is out of resources (ENFILE)" );
  n_mq2 = mq_open( Get_Queue_Name(i), O_CREAT, O_RDONLY, NULL );
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, ENFILE,  "mq_open errno ENFILE"); 

  /*
   * Unlink and Close .
   */

  puts( "mq_close and mq_unlink (mq3...mqn) - SUCCESSFUL" );
  for (i = size; i < CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES; i++) {

    status = mq_close( mqs[i] );
    fatal_directive_status( status, 0, "mq_close message queue"); 

    status = mq_unlink( Get_Queue_Name(i) );
    fatal_directive_status( status, 0, "mq_unlink message queue"); 
  }
}

void validate_mq_unlink_error_codes(
  mqd_t   *mqs,      /* Must be large enough for Maximum to be opened. */
  int      size      /* Number still open */
)
{
  int             status;

  /*
   * XXX - EACCES Permission Denied
   */

  /*
   * XXX ENAMETOOLONG - Not checked in either sem_unlink or mq_unlink is
   *                    this an error? 
  for ( i=0; i< PATH_MAX+1; i++ )
    name[i] = 'N';
  puts( "mq_open - Open with too long of a name (ENAMETOOLONG)" );
  n_mq2 = mq_open( Get_Too_Long_Name(), O_CREAT, O_RDONLY, NULL );
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, ENAMETOOLONG,  "mq_open errno ENAMETOOLONG"); 
  */
  
  puts( "mq_unlink - UNSUCCESSFUL (ENOENT)" );
  status = mq_unlink(Get_Queue_Name(size));
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, ENOENT, "mq_unlink errno ENOENT");
}

void validate_mq_close_error_codes(
  mqd_t   *mqs,      /* Must be large enough for Maximum to be opened. */
  int      size      /* Number still open */
)
{
  int             status;

  puts( "mq_close - UNSUCCESSFUL (EBADF)" );
  status = mq_close(mqs[size]);
  fatal_directive_status( status, -1, "mq_close error return status");
  fatal_directive_status( errno, EBADF, "mq_close errno EBADF");

}

void *POSIX_Init(
  void *argument
)
{
  int             status;
  int             value;
  int             i;
  mqd_t           mqs[CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES];
  mqd_t           mq2;
  mqd_t           n_mq1;
  mqd_t           n_mq2;
  struct timespec waittime;
  char            failure_msg[80];
  struct mq_attr  attr;

  puts( "\n\n*** POSIX MESSAGE QUEUE TEST ***" );

  validate_mq_open_error_codes( mqs, 2 );
  validate_mq_unlink_error_codes( mqs, 2 );
  validate_mq_close_error_codes( mqs, 2 );

  /*
   * Validate a second open returns the same message queue.
   */

  puts( "mq_open - Open an existing mq ( same id )" );
  n_mq2 = mq_open("mq1", 0 );
  fatal_directive_status( 
    (int) n_mq2, (int ) mqs[0], "mq_open error return status" );
  
  /*
   * Unlink the message queue, then verify an open of the same name produces a 
   * different message queue.
   */

  puts( "mq_unlink - mq1 SUCCESSFUL" );
  status = mq_unlink( "mq1" );
  fatal_directive_status( status, 0, "mq_unlink locked message queue"); 

  puts( "mq_open - Reopen mq1 SUCCESSFUL with a different id" );
  n_mq2 = mq_open( "mq1", O_CREAT | O_EXCL, 00777, NULL);
  assert( n_mq2 != (-1) );
  assert( n_mq2 != n_mq1 );

  /*
   * Validate it n_mq2 (the last open for mq1 name can be
   * correctly closed and unlinked.
   */

  puts( "Init: mq_unlink - mq1 (2) SUCCESSFUL" );
  status = mq_unlink( "mq1" );
  fatal_directive_status( status, 0, "mq_unlink locked message queue"); 

  puts( "Init: mq_close (2) - SUCCESSFUL" );
  status = mq_close( n_mq2 );
  fatal_directive_status( status, 0, "mq_close message queue"); 


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
  status = mq_unlink("mq3");
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, ENOENT, "mq_unlink errno ENOENT");
  assert( (status == -1) && (errno == ENOENT) );



  /*
   * Validate we can wait on a message queue opened with mq_open.
   */
#if (0) /* XXX FIX ME */
  puts( "Init: mq_wait on mq1" );
  status = mq_receive(n_mq1);
  fatal_directive_status( status, 0, "mq_wait opened message queue"); 
#endif


  puts( "*** END OF POSIX MESSAGE QUEUE TEST ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
