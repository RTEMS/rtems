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

typedef enum {
  DEFAULT_SIZE_TYPE,
  TEST_SIZE_TYPE,
  MAX_SIZE,
  TYPES_OF_TEST_SIZES
} TEST_MQ_SIZE_TYPES;


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
  n_mq2 = mq_open("mq2", O_CREAT | O_RDONLY, 0x777, &attr);
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, EINVAL,  "mq_open errno EINVAL"); 

  attr.mq_msgsize = -1;
  puts( "mq_open - Create with msgsize (-1) (EINVAL)" );
  n_mq2 = mq_open("mq2", O_CREAT | O_RDONLY, 0x777, &attr);
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, EINVAL,  "mq_open errno EINVAL"); 

  puts( "mq_open - Open new mq without create flag (ENOENT)" );
  n_mq2 = mq_open("mq3", O_EXCL | O_RDONLY, 0x777, NULL);
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
  n_mq2 = mq_open( Get_Too_Long_Name(), O_CREAT | O_RDONLY, 0x777, NULL );
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, ENAMETOOLONG,  "mq_open errno ENAMETOOLONG"); 
  
  /*
   * Open maximum number of message queues
   */

  puts( "mq_open - SUCCESSFUL" );
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES; i++) {
    mqs[i] = mq_open( Get_Queue_Name(i), O_CREAT | O_RDWR, 0x777, NULL );
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
  n_mq2 = mq_open("mq1", O_CREAT | O_WRONLY, 0x777, NULL);
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, EACCES,  "mq_open errno EACCES");
   */

  puts( "mq_open - Create an Existing mq (EEXIST)" );
  n_mq2 = mq_open("mq1", O_CREAT | O_EXCL | O_RDONLY, 0x777, NULL);
  fatal_directive_status( 
    (int) n_mq2, (int ) (-1), "mq_open error return status" );
  fatal_directive_status( errno, EEXIST,  "mq_open errno EEXIST");


  /*
   * XXX EMFILE  - Too many message queues open
   */

  puts( "mq_open - system is out of resources (ENFILE)" );
  n_mq2 = mq_open( Get_Queue_Name(i), O_CREAT | O_RDONLY, 0x777, NULL );
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
  mqd_t   *mqs,      
  int      size      /* Number still open in mqs */
)
{
  int             status;

  /*
   * XXX - EACCES Permission Denied
   */

  /*
   * XXX ENAMETOOLONG - Not checked in either sem_unlink or mq_unlink is
   *                    this an error? 
   */

  puts( "mq_unlink - mq_unlink with too long of a name (ENAMETOOLONG)" );
  status = mq_unlink( Get_Too_Long_Name() );
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, ENAMETOOLONG, "mq_unlink errno ENAMETOOLONG");
  
  puts( "mq_unlink - UNSUCCESSFUL (ENOENT)" );
  status = mq_unlink(Get_Queue_Name(size));
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, ENOENT, "mq_unlink errno ENOENT");

  /*
   * XXX - These errors are not in the POSIX manual but may occur.
   */

  puts( "mq_unlink (NULL) - EINVAL" );
  status = mq_unlink( NULL );
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, EINVAL, "mq_unlink errno value"); 

  puts( "mq_unlink (\"\") - EINVAL" );
  status = mq_unlink( "" );
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, EINVAL, "mq_unlink errno value"); 
}

void validate_mq_close_error_codes(
  mqd_t   *mqs,      
  int      size      /* Number still open in mqs */
)
{
  int             status;

  puts( "mq_close - UNSUCCESSFUL (EBADF)" );
  status = mq_close(mqs[size]);
  fatal_directive_status( status, -1, "mq_close error return status");
  fatal_directive_status( errno, EBADF, "mq_close errno EBADF");
}
  
/*
 * Returns the number of messages queued after the test on the
 * first queue.
 */

int validate_mq_send_error_codes( 
  mqd_t   *mqs,
  int      size      /* Number still open in mqs */
)
{
  int             status;
  int             i;
  mqd_t           n_mq1;
  struct mq_attr  attr;

  attr.mq_maxmsg  = 3;
  attr.mq_msgsize = 8;

  /*
   * XXX - EBADF  Not a valid message descriptor.
   *       Write to a invalid message descriptor
   * XXX - Write to a read only queue
   */

  puts( "mq_send - Closed message queue (EBADF)" );
  status = mq_send( mqs[size], "", 1, 0 );
  fatal_directive_status( status, -1, "mq_send error return status");
  fatal_directive_status( errno, EBADF, "mq_send errno EBADF");

  puts( "mq_open - Open a read only queue" );
  n_mq1 = mq_open("read_only", O_CREAT | O_RDONLY, 0x777, &attr);
  assert( n_mq1 != (-1) );
  /*XXX - Isn't there a more general check */ 

  puts( "mq_send - Read only message queue (EBADF)" );
  status = mq_send( n_mq1, "", 1, 0 );
  fatal_directive_status( status, -1, "mq_send error return status");
  fatal_directive_status( errno, EBADF, "mq_send errno EBADF");

  status = mq_close( n_mq1 );
  fatal_directive_status( status, 0, "mq_close message queue"); 

  status = mq_unlink( "read_only" );
  fatal_directive_status( status, 0, "mq_unlink message queue"); 

  /*
   * XXX - EINTR
   *       Signal interrupted the call.

  puts( "mq_send - UNSUCCESSFUL (EINTR)" );
  status = mq_send( mqs, "", 0xffff, 0 );
  fatal_directive_status( status, -1, "mq_send error return status");
  fatal_directive_status( errno, E, "mq_send errno E");
   */

  /*
   * XXX - EINVAL priority is out of range.
   */

  puts( "mq_send - Priority out of range (EINVAL)" );
  status = mq_send( mqs[0], "", 1, MQ_PRIO_MAX + 1 );
  fatal_directive_status( status, -1, "mq_send error return status");
  fatal_directive_status( errno, EINVAL, "mq_send errno EINVAL");

  /*
   * XXX - EMSGSIZE - Message size larger than msg_len
   */

  puts( "mq_send - Message longer than msg_len (EMSGSIZE)" );
  status = mq_send( mqs[0], "", 0xffff, 0 );
  fatal_directive_status( status, -1, "mq_send error return status");
  fatal_directive_status( errno, EMSGSIZE, "mq_send errno EMSGSIZE");

  /*
   * ENOSYS - send is supported should never happen.
   */


  /*
   * XXX - EAGAIN 
   *       O_NONBLOCK and message queue is full.
   *       This is validated in the read/write test.
   */

  i=0;
  do {
    status = mq_send( mqs[0], "", 1, 0 );
    i++;
  } while (status == 0);
  fatal_directive_status( status, -1, "mq_send error return status");
  fatal_directive_status( errno, EAGAIN, "mq_send errno EAGAIN");

  return i-1;
}

void validate_mq_receive_error_codes( 
  mqd_t   *mqs,      
  int      size      /* Number still open in mqs */
)
{
  int             status;

  /*
   * EAGAIN - 
   */

  /*
   * EBADF - 
   */

  /*
   * EMSGSIZE - 
   */

  /*
   * EINTR - 
   */

  /*
   * EBADMSG - a data corruption problem.
   *  XXX - Can not cause.
   */

  /*
  puts( "mq_ - UNSUCCESSFUL ()" );
  status = mq_(  );
  fatal_directive_status( status, -1, "mq_ error return status");
  fatal_directive_status( errno, E, "mq_c errno E");

  */
  /*
   * ENOSYS - 
   */

}

void non_blocking_mq_read_write(
  mqd_t   *mqs,      
  int      size      /* Number still open in mqs */
)
{
  /*
  int         status;
  char       *messages[] = {
    "Msg 1",
    "Test 2",
    "12345678901234567890"
  };

  status = mq_send( mqs[0], messages[0], strlen( messages[0] ), 0 );
  fatal_directive_status( status, 0, "mq_send error return status" );
    
  puts( "mq_send - UNSUCCESSFUL ()" );
  do {
    status = mq_send(  );
  fatal_directive_status( status, -1, "mq_send error return status");
  fatal_directive_status( errno, E, "mq_send errno E");
  }
  */
}

void *POSIX_Init(
  void *argument
)
{
  int             status;
  mqd_t           mqs[CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES];
  mqd_t           n_mq1;
  mqd_t           n_mq2;
  char           *messages[] = {
    "Msg 1",
    "Test 2",
    "12345678901234567890"
  };

  puts( "\n\n*** POSIX MESSAGE QUEUE TEST ***" );

  validate_mq_open_error_codes( mqs, 2 );
  validate_mq_unlink_error_codes( mqs, 2 );
  validate_mq_close_error_codes( mqs, 2 );

  validate_mq_send_error_codes( mqs, 2 );
  validate_mq_receive_error_codes( mqs, 2 );


  /*
   * Validate a second open returns the same message queue.
   */

  puts( "mq_open - Open an existing mq ( same id )" );
  n_mq1 = mq_open("mq1", 0 );
  fatal_directive_status( 
    (int) n_mq1, (int ) mqs[0], "mq_open error return status" );
  
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
   * Validate it "mq1" can be closed and unlinked.
   */

  puts( "mq_unlink - mq1 SUCCESSFUL" );
  status = mq_unlink( "mq1" );
  fatal_directive_status( status, 0, "mq_unlink locked message queue"); 

  puts( "mq_close mq1 - SUCCESSFUL" );
  status = mq_close( n_mq2 );
  fatal_directive_status( status, 0, "mq_close message queue"); 
  status = mq_close( n_mq1 );
  fatal_directive_status( status, 0, "mq_close message queue"); 
  status = mq_close( mqs[0] );
  fatal_directive_status( status, 0, "mq_close message queue"); 

  puts( "mq_unlink - UNSUCCESSFUL (ENOENT)" );
  status = mq_unlink("mq1");
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, ENOENT, "mq_close errno EINVAL");

  /*
   * XXX - Cant' create location OBJECTS_ERROR or OBJECTS_REMOTE.
   *       mq_close and mq_unlink.
   * XXX - Don't think we need this save until yellow line tested.
  puts( "Init: mq_unlink - UNSUCCESSFUL (ENOENT)" );
  status = mq_unlink("mq3");
  fatal_directive_status( status, -1, "mq_unlink error return status");
  fatal_directive_status( errno, ENOENT, "mq_unlink errno ENOENT");
  assert( (status == -1) && (errno == ENOENT) );
  */


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


