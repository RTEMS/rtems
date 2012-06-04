/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include <sched.h>
#include <fcntl.h>
#include <time.h>
#include <tmacros.h>
#include <signal.h>   /* signal facilities */
#include "test_support.h"

/* forward declarations to avoid warnings */
void Start_Test(char *description);
void Validate_attributes(mqd_t mq, int oflag, int msg_count);
char *Build_Queue_Name(int i);
void open_test_queues(void);
void validate_mq_open_error_codes(void);
void validate_mq_unlink_error_codes(void);
void validate_mq_close_error_codes(void);
void validate_mq_getattr_error_codes(void);
void Send_msg_to_que(int que, int msg);
void Show_send_msg_to_que(char *task_name, int que, int msg);
void verify_queues_full(char *task_name);
void verify_queues_empty(char *task_name);
int empty_message_queues(char *task_name);
int fill_message_queues(char *task_name);
void Read_msg_from_que(int que, int msg);
int validate_mq_send_error_codes(void);
void validate_mq_receive_error_codes(void);
void verify_open_functionality(void);
void verify_unlink_functionality(void);
void verify_close_functionality(void);
void verify_timed_send_queue(int que, int is_blocking);
void verify_timed_send(void);
void verify_timed_receive_queue(char *task_name, int que, int is_blocking);
void verify_timed_receive(void);
void wait_for_signal(sigset_t *waitset, int sec, int expect_signal);
void verify_notify(void);
void verify_with_threads(void);
void verify_timedout_mq_timedreceive(char *task_name, int que, int is_blocking);
void verify_timedout_mq_timedsend(int que, int is_blocking);
void verify_timed_receive(void);
void validate_mq_setattr(void);
void verify_timedout_mq_timedreceive(
  char *task_name, int que, int is_blocking);
void verify_mq_receive(void);
void verify_timedout_mq_timedsend(int que, int is_blocking);
void verify_mq_send(void);
void verify_timed_receive(void);

#define fatal_posix_mqd( _ptr, _msg ) \
  if ( (_ptr != (mqd_t) -1) ) { \
    check_dispatch_disable_level( 0 ); \
    printf( "\n%s FAILED -- expected (-1) got (%" PRId32 " - %d/%s)\n", \
	    (_msg), _ptr, errno, strerror(errno) ); \
    FLUSH_OUTPUT(); \
    rtems_test_exit( -1 ); \
  }

typedef struct {
  char         msg[ 50 ];
  int          size;
  unsigned int priority;
} Test_Message_t;

Test_Message_t Predefined_Msgs[MAXMSG+1];
Test_Message_t Predefined_Msgs[MAXMSG+1] = {
  { "12345678",   9, MQ_PRIO_MAX-1 },  /* Max Length Message med  */
  { "",           1, 1             },  /* NULL  Message      low  */
  { "Last",       5, MQ_PRIO_MAX   },  /* Queue Full Message hi   */
  { "No Message", 0, MQ_PRIO_MAX-1 },  /* 0 length Message   med  */
  { "1",          2, 0             },  /* Cause Overflow Behavior */
};
int Priority_Order[MAXMSG+1] = { 2, 0, 3, 1, MAXMSG };


typedef struct {
  mqd_t              mq;
  Test_Queue_Types   index;
  char              *name;
  int                oflag;
  int                maxmsg;
  int                msgsize;
  int                count;
} Test_queue_type;

Test_queue_type Test_q[ NUMBER_OF_TEST_QUEUES + 1 ] =
{
  { 0, 0, "Qread",    ( O_CREAT | O_RDONLY | O_NONBLOCK ), MAXMSG, MSGSIZE, 0 },
  { 0, 1, "Qwrite",   ( O_CREAT | O_WRONLY | O_NONBLOCK ), MAXMSG, MSGSIZE, 0 },
  { 0, 2, "Qnoblock", ( O_CREAT | O_RDWR   | O_NONBLOCK ), MAXMSG, MSGSIZE, 0 },
  { 0, 3, "Qblock",   ( O_CREAT | O_RDWR )               , MAXMSG, MSGSIZE, 0 },
  { 0, 4, "Qdefault", ( O_CREAT | O_RDWR )               , 10,     16,      0 },
  { 0, 5, "mq6",      ( O_CREAT | O_WRONLY | O_NONBLOCK ), MAXMSG, MSGSIZE, 0 },
  { 0, 6, "Qblock",   (           O_RDWR )               , MAXMSG, MSGSIZE, 0 },
};

#define RW_NAME             Test_q[ RW_QUEUE ].name
#define DEFAULT_NAME        Test_q[ DEFAULT_RW ].name
#define RD_NAME             Test_q[ RD_QUEUE ].name
#define WR_NAME             Test_q[ WR_QUEUE ].name
#define BLOCKING_NAME       Test_q[ BLOCKING ].name
#define CLOSED_NAME         Test_q[ CLOSED ].name

#define RW_ATTR         Test_q[ RW_QUEUE ].oflag
#define DEFAULT_ATTR    Test_q[ DEFAULT_RW ].oflag
#define RD_ATTR         Test_q[ RD_QUEUE ].oflag
#define WR_ATTR         Test_q[ WR_QUEUE ].oflag
#define BLOCK_ATTR      Test_q[ BLOCKING ].oflag
#define CLOSED_ATTR     Test_q[ CLOSED ].oflag

/*
 * Outputs a header at each test section.
 */
void Start_Test(
  char *description
)
{
  printf( "_______________%s\n", description );
}


void Validate_attributes(
  mqd_t  mq,
  int    oflag,
  int    msg_count
)
{
  int             status;
  struct mq_attr  attr;

  status = mq_getattr( mq, &attr );
  fatal_posix_service_status( status, 0, "mq_getattr valid return status");

  if ( mq != Test_q[ DEFAULT_RW ].mq ){
    fatal_int_service_status((int)attr.mq_maxmsg, MAXMSG, "maxmsg attribute" );
    fatal_int_service_status((int)attr.mq_msgsize,MSGSIZE,"msgsize attribute");
  }

  fatal_int_service_status((int)attr.mq_curmsgs, msg_count, "count attribute" );
  fatal_int_service_status((int)attr.mq_flags, oflag, "flag attribute" );
}

#define Get_Queue_Name( i )  Test_q[i].name
char *Build_Queue_Name(int i)
{
  static char Queue_Name[PATH_MAX + 2];

  sprintf(Queue_Name,"mq%d", i+1 );
  return Queue_Name;
}

void open_test_queues(void)
{
  struct mq_attr   attr;
  int              status;
  Test_queue_type *tq;
  int              que;

  attr.mq_maxmsg  = MAXMSG;
  attr.mq_msgsize = MSGSIZE;

  puts( "Init: Open Test Queues" );

  for( que = 0; que < NUMBER_OF_TEST_QUEUES+1; que++ ) {

    tq = &Test_q[ que ];
    if ( que == DEFAULT_RW)
      Test_q[que].mq = mq_open( tq->name, tq->oflag, 0x777, NULL );
    else
      Test_q[que].mq = mq_open( tq->name, tq->oflag, 0x777, &attr );

    rtems_test_assert( Test_q[que].mq != (-1) );
  }

  status = mq_close( Test_q[NUMBER_OF_TEST_QUEUES].mq );
  fatal_posix_service_status( status, 0, "mq_close duplicate message queue");
  status = mq_close( Test_q[CLOSED].mq );
  fatal_posix_service_status( status, 0, "mq_close message queue");
  status = mq_unlink( CLOSED_NAME );
  fatal_posix_service_status( status, 0, "mq_unlink message queue");
}

/*
 * Opens CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES then leaves size queues
 * opened but closes the rest.
 */

void validate_mq_open_error_codes(void)
{
  int             i;
  mqd_t           n_mq2;
  struct mq_attr  attr;
  int             status;
  mqd_t           open_mq[CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES + 1];

  attr.mq_maxmsg  = MAXMSG;
  attr.mq_msgsize = MSGSIZE;

  Start_Test( "mq_open errors" );

  /*
   * XXX EINVAL - inappropriate name was given for the message queue
   */

  /*
   * EINVAL - Create with negative maxmsg.
   */

  attr.mq_maxmsg = -1;
  puts( "Init: mq_open - Create with maxmsg (-1) (EINVAL)" );
  n_mq2 = mq_open( "mq2", O_CREAT | O_RDONLY, 0x777, &attr);
  fatal_posix_mqd( n_mq2, "mq_open error return status" );
  fatal_posix_service_status( errno, EINVAL,  "mq_open errno EINVAL");
  attr.mq_maxmsg  = MAXMSG;

  /*
   * EINVAL - Create withnegative msgsize.
   */

  attr.mq_msgsize = -1;
  puts( "Init: mq_open - Create with msgsize (-1) (EINVAL)" );
  n_mq2 = mq_open( "mq2", O_CREAT | O_RDONLY, 0x777, &attr);
  fatal_posix_mqd( n_mq2, "mq_open error return status" );
  fatal_posix_service_status( errno, EINVAL,  "mq_open errno EINVAL");
  attr.mq_msgsize = MSGSIZE;

  /*
   * ENOENT - Open a non-created file.
   */

  puts( "Init: mq_open - Open new mq without create flag (ENOENT)" );
  n_mq2 = mq_open( "mq3", O_EXCL | O_RDONLY, 0x777, NULL);
  fatal_posix_mqd( n_mq2, "mq_open error return status" );
  fatal_posix_service_status( errno, ENOENT,  "mq_open errno ENOENT");

  /*
   * XXX EINTR  - call was interrupted by a signal
   */

  /*
   * ENAMETOOLONG - Give a name greater than PATH_MAX.
   */

  puts( "Init: mq_open - Open with too long of a name (ENAMETOOLONG)" );
  n_mq2 = mq_open( Get_Too_Long_Name(), O_CREAT | O_RDONLY, 0x777, NULL );
  fatal_posix_mqd( n_mq2, "mq_open error return status" );
  fatal_posix_service_status( errno, ENAMETOOLONG, "mq_open errno ENAMETOOLONG");

  /*
   * XXX - ENAMETOOLONG - Give a name greater than NAME_MAX
   *       Per implementation not possible.
   */

  /*
   * EEXIST - Create an existing queue.
   */

  puts( "Init: mq_open - Create an Existing mq (EEXIST)" );
  open_mq[0] = mq_open(
    Build_Queue_Name(0), O_CREAT | O_RDWR | O_NONBLOCK, 0x777, NULL );
  rtems_test_assert( open_mq[0] != (-1) );

  n_mq2 = mq_open(
    Build_Queue_Name(0), O_CREAT | O_EXCL | O_RDONLY, 0x777, NULL);
  fatal_posix_mqd( n_mq2, "mq_open error return status" );
  fatal_posix_service_status( errno, EEXIST,  "mq_open errno EEXIST");

  status = mq_unlink( Build_Queue_Name(0) );
  fatal_posix_service_status( status, 0, "mq_unlink message queue");

  status = mq_close( open_mq[0]);
  fatal_posix_service_status( status, 0, "mq_close message queue");

  /*
   * Open maximum number of message queues
   */

  puts( "Init: mq_open - SUCCESSFUL" );
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES; i++) {
    open_mq[i] = mq_open(
      Build_Queue_Name(i), O_CREAT | O_RDWR | O_NONBLOCK, 0x777, NULL );
    rtems_test_assert( open_mq[i] != (-1) );
    rtems_test_assert( open_mq[i] );
    /*XXX - Isn't there a more general check */
/* JRS     printf( "mq_open 0x%x %s\n", open_mq[i], Build_Queue_Name(i) ); */
  }

  /*
   * XXX EACCES - permission to create is denied.
   */

  /*
   * XXX EACCES - queue exists permissions specified by o_flag are denied.
   */

  /*
   * XXX EMFILE  - Too many message queues in use by the process
   */

  /*
   * ENFILE -  Too many message queues open in the system
   */

  puts( "Init: mq_open - system is out of resources (ENFILE)" );
  n_mq2 = mq_open( Build_Queue_Name(i), O_CREAT | O_RDONLY, 0x777, NULL );
  fatal_posix_mqd( n_mq2, "mq_open error return status" );
  fatal_posix_service_status( errno, ENFILE,  "mq_open errno ENFILE");

  /*
   * Unlink and Close all queues.
   */

  puts( "Init: mq_close and mq_unlink (mq3...mqn) - SUCCESSFUL" );
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES; i++) {

    status = mq_close( open_mq[i]);
    fatal_posix_service_status( status, 0, "mq_close message queue");

    status = mq_unlink( Build_Queue_Name(i) );
    if ( status == -1 )
      perror( "mq_unlink" );
    fatal_posix_service_status( status, 0, "mq_unlink message queue");
    /* JRS printf( "mq_close/mq_unlink 0x%x %s\n", open_mq[i], Build_Queue_Name(i) ); */
  }
}

void validate_mq_unlink_error_codes(void)
{
  int             status;

  Start_Test( "mq_unlink errors" );

  /*
   * XXX - EACCES Permission Denied
   */

  /*
   * ENAMETOOLONG - Give a name greater than PATH_MAX.
   */

  puts( "Init: mq_unlink - mq_unlink with too long of a name (ENAMETOOLONG)" );
  status = mq_unlink( Get_Too_Long_Name() );
  fatal_posix_service_status( status, -1, "mq_unlink error return status");
  fatal_posix_service_status( errno, ENAMETOOLONG, "mq_unlink errno ENAMETOOLONG");

  /*
   * XXX - ENAMETOOLONG - Give a name greater than NAME_MAX
   *       Per implementation not possible.
   */

  /*
   *  ENOENT - Unlink an unopened queue
   */

  puts( "Init: mq_unlink - A Queue not opened  (ENOENT)" );
  status = mq_unlink( CLOSED_NAME );
  fatal_posix_service_status( status, -1, "mq_unlink error return status");
  fatal_posix_service_status( errno, ENOENT, "mq_unlink errno ENOENT");

  /*
   * XXX - The following were not listed in the POSIX document as
   *       possible errors.  Under other commands the EINVAL is
   *       given for these conditions.
   */

  /*
   *  EINVAL - Unlink a queue with no name
   */

  puts( "Init: mq_unlink (NULL) - EINVAL" );
  status = mq_unlink( NULL );
  fatal_posix_service_status( status, -1, "mq_unlink error return status");
  fatal_posix_service_status( errno, EINVAL, "mq_unlink errno value");

  /*
   *  EINVAL - Unlink a queue with a null name
   */

  puts( "Init: mq_unlink (\"\") - EINVAL" );
  status = mq_unlink( "" );
  fatal_posix_service_status( status, -1, "mq_unlink error return status");
  fatal_posix_service_status( errno, EINVAL, "mq_unlink errno value");
}

void validate_mq_close_error_codes(void)
{
  int             status;

  Start_Test( "mq_close errors" );

  /*
   * EBADF - Close a queue that is not open.
   */

  puts( "Init: mq_close - unopened queue (EBADF)" );
  status = mq_close( Test_q[CLOSED].mq );
  fatal_posix_service_status( status, -1, "mq_close error return status");
  fatal_posix_service_status( errno, EBADF, "mq_close errno EBADF");
}


void validate_mq_getattr_error_codes(void)
{
  struct mq_attr  attr;
  int             status;

  Start_Test( "mq_getattr errors" );

  /*
   * EBADF - Get the attributes from a closed queue.
   */

  puts( "Init: mq_getattr - unopened queue (EBADF)" );
  status = mq_getattr( Test_q[CLOSED].mq, &attr );
  fatal_posix_service_status( status, -1, "mq_close error return status");
  fatal_posix_service_status( errno, EBADF, "mq_close errno EBADF");

  /*
   * XXX - The following are not listed in the POSIX manual but
   *       may occur.
   */

  /*
   * EINVAL - NULL attributes
   */

  puts( "Init: mq_getattr - NULL attributes (EINVAL)" );
  status = mq_getattr( Test_q[RW_QUEUE].mq, NULL );
  fatal_posix_service_status( status, -1, "mq_close error return status");
  fatal_posix_service_status( errno, EINVAL, "mq_close errno EINVAL");

}


void Send_msg_to_que(
  int que,
  int msg
)
{
  Test_Message_t *ptr = &Predefined_Msgs[msg];
  int             status;

  status = mq_send( Test_q[que].mq, ptr->msg, ptr->size , ptr->priority );
  fatal_posix_service_status( status, 0, "mq_send valid return status");
  Test_q[que].count++;
}

void Show_send_msg_to_que(
  char *task_name,
  int   que,
  int   msg
)
{
  Test_Message_t *ptr = &Predefined_Msgs[msg];
  printf( "%s mq_send -  to %s msg: %s priority %d\n",
    task_name, Test_q[que].name, ptr->msg, ptr->priority);
  Send_msg_to_que( que, msg );
}

void verify_queues_full(
  char *task_name
)
{
  int          que;

  /*
   * Validate that the queues are full.
   */

  printf( "%s Verify Queues are full\n", task_name );
  for( que = RW_QUEUE; que < CLOSED; que++ )
    Validate_attributes( Test_q[que].mq, Test_q[que].oflag, Test_q[que].count );

}
void verify_queues_empty(
  char *task_name
)
{
  int             que;

  printf( "%s Verify Queues are empty\n", task_name );
  for( que = RW_QUEUE; que < CLOSED; que++ )
    Validate_attributes( Test_q[que].mq, Test_q[que].oflag, 0 );
}

int fill_message_queues(
  char *task_name
)
{
  int             msg;
  int             que;


  verify_queues_empty( task_name );

  /*
   * Fill Queue with predefined messages.
   */

  printf( "%s Fill Queues with messages\n", task_name );
  for(msg=0; msg<MAXMSG; msg++){
    for( que = RW_QUEUE; que < CLOSED; que++ ) {
      Send_msg_to_que( que, msg );
    }
  }

  verify_queues_full( "Init:" );
  return msg;
}


void Read_msg_from_que(
  int que,
  int msg
)
{
  unsigned int    priority;
  Test_Message_t *ptr;
  int             status;
  char            message[100];
  char            err_msg[100];

  ptr = &Predefined_Msgs[msg];
  status = mq_receive(Test_q[ que ].mq, message, 100, &priority );
  Test_q[que].count--;

  sprintf( err_msg, "%s msg %s size failure", Test_q[ que ].name, ptr->msg );
  fatal_int_service_status( status, ptr->size, err_msg );

  rtems_test_assert( !strcmp( message, ptr->msg ) );
  strcpy( message, "No Message" );

  sprintf( err_msg,"%s msg %s size failure", Test_q[ que ].name, ptr->msg );
  fatal_int_service_status(priority, ptr->priority, err_msg );
}

int empty_message_queues(
  char *task_name
)
{
  int que;
  int i;

  printf( "%s Empty all Queues\n", task_name );
  for( que = RW_QUEUE; que < CLOSED; que++ ) {
    for(i=0; Test_q[que].count != 0; i++ )
      Read_msg_from_que( que,  Priority_Order[i] );

    Validate_attributes( Test_q[ que].mq, Test_q[ que ].oflag, 0 );
  }
  return 0;
}

/*
 * Returns the number of messages queued after the test on the
 * first queue.
 */
int validate_mq_send_error_codes(void)
{
  int             status;
  int             i;
  char           *str;

  Start_Test( "mq_send errors" );

  /*
   * EBADF - Write to a closed queue.
   */

  puts( "Init: mq_send - Closed message queue (EBADF)" );
  status = mq_send( Test_q[CLOSED].mq, "", 1, 0 );
  fatal_posix_service_status( status, -1, "mq_send error return status");
  fatal_posix_service_status( errno, EBADF, "mq_send errno EBADF");

  /*
   * EBADF - Write to a read only  queue.
   */

  puts( "Init: mq_send - Read only message queue (EBADF)" );
  status = mq_send( Test_q[ RD_QUEUE ].mq, "", 1, 0 );
  fatal_posix_service_status( status, -1, "mq_send error return status");
  fatal_posix_service_status( errno, EBADF, "mq_send errno EBADF");

  /*
   * XXX - EINTR      Signal interrupted the call.
   *
  puts( "Init: mq_send - UNSUCCESSFUL (EINTR)" );
  status = mq_send( Test_q, "", 0xffff, 0 );
  fatal_posix_service_status( status, -1, "mq_send error return status");
  fatal_posix_service_status( errno, E, "mq_send errno E");
   */

  /*
   * EINVAL priority is out of range.
   */

  puts( "Init: mq_send - Priority out of range (EINVAL)" );
  status = mq_send( Test_q[ RW_QUEUE ].mq, "", 1, MQ_PRIO_MAX + 1 );
  fatal_posix_service_status( status, -1, "mq_send error return status");
  fatal_posix_service_status( errno, EINVAL, "mq_send errno EINVAL");

  /*
   *  EMSGSIZE - Message size larger than msg_len
   *             Validates that msgsize is stored correctly.
   */

  puts( "Init: mq_send - Message longer than msg_len (EMSGSIZE)" );
  status = mq_send( Test_q[ RW_QUEUE ].mq, "", MSGSIZE+1, 0 );
  fatal_posix_service_status( status, -1, "mq_send error return status");
  fatal_posix_service_status( errno, EMSGSIZE, "mq_send errno EMSGSIZE");

  i = fill_message_queues( "Init:" );

  /*
   * ENOSYS - send not supported
  puts( "Init: mq_send - Blocking Queue overflow (ENOSYS)" );
  status = mq_send( n_mq1, Predefined_Msgs[i], 0, 0 );
  fatal_posix_service_status( status, -1, "mq_send error return status");
  fatal_posix_service_status( errno, EBADF, "mq_send errno EBADF");

  status = mq_close( n_mq1 );
  fatal_posix_service_status( status, 0, "mq_close message queue");

  status = mq_unlink( "read_only" );
  fatal_posix_service_status( status, 0, "mq_unlink message queue");
   */

  /*
   * EAGAIN - O_NONBLOCK and message queue is full.
   */

  puts( "Init: mq_send - on a FULL non-blocking queue with (EAGAIN)" );
  str = Predefined_Msgs[i].msg;
  status = mq_send(Test_q[RW_QUEUE].mq, str, 0, 0 );
  fatal_posix_service_status( status, -1, "mq_send error return status");
  fatal_posix_service_status( errno, EAGAIN, "mq_send errno EAGAIN");

  return i-1;
}

void validate_mq_receive_error_codes(void)
{
  int            status;
  char           message[100];
  unsigned int   priority;

  Start_Test( "mq_receive errors"  );

  /*
   * EBADF - Not A Valid Message Queue
   */

  puts( "Init: mq_receive - Unopened message queue (EBADF)" );
  status = mq_receive( Test_q[CLOSED].mq, message, 100, &priority );
  fatal_posix_service_status( status, -1, "mq_ error return status");
  fatal_posix_service_status( errno, EBADF, "mq_receive errno EBADF");

  /*
   * EBADF - Queue not opened to read
   */

  puts( "Init: mq_receive - Write only queue (EBADF)" );
  status = mq_receive( Test_q[WR_QUEUE].mq, message, 100, &priority  );
  fatal_posix_service_status( status, -1, "mq_ error return status");
  fatal_posix_service_status( errno, EBADF, "mq_receive errno EBADF");

  /*
   * EMSGSIZE - Size is less than the message size attribute
   */

  puts( "Init: mq_receive - Size is less than the message (EMSGSIZE)" );
  status = mq_receive(
    Test_q[RW_QUEUE].mq, message, Predefined_Msgs[0].size-1, &priority );
  fatal_posix_service_status( status, -1, "mq_ error return status");
  fatal_posix_service_status( errno, EMSGSIZE, "mq_receive errno EMSGSIZE");


  /*
   * EAGAIN - O_NONBLOCK and Queue is empty
   */
  verify_queues_full( "Init:" );
  empty_message_queues( "Init:" );

  puts( "Init: mq_receive - Queue is empty (EAGAIN)" );
  status = mq_receive( Test_q[RW_QUEUE].mq, message, 100, &priority );
  fatal_posix_service_status( status, -1, "mq_ error return status");
  fatal_posix_service_status( errno, EAGAIN, "mq_receive errno EAGAIN");

  /*
   * XXX - EINTR - Interrupted by a signal
   */

  /*
   * XXX - EBADMSG - a data corruption problem.
   */

  /*
   * XXX - ENOSYS - mq_receive not supported
   */
}

void verify_open_functionality(void)
{
#if 0
  mqd_t           n_mq;
#endif

  Start_Test( "mq_open functionality" );

  /*
   * Validate a second open returns the same message queue.
   */

#if 0
  puts( "Init: mq_open - Open an existing mq ( same id )" );
  n_mq = mq_open( RD_NAME, 0 );
  fatal_posix_service_status(
  rtems_test_assert( n_mq == Test_q[RD_QUEUE].mq );
#endif
}

void verify_unlink_functionality(void)
{
  mqd_t           n_mq;
  int             status;

  Start_Test( "mq_unlink functionality" );

  /*
   * Unlink the message queue, then verify an open of the same name produces a
   * different message queue.
   */

  puts( "Init: Unlink and Open without closing SUCCESSFUL" );
  status = mq_unlink( DEFAULT_NAME );
  fatal_posix_service_status( status, 0, "mq_unlink locked message queue");

  n_mq = mq_open( DEFAULT_NAME, DEFAULT_ATTR, 0x777, NULL );
  rtems_test_assert( n_mq != (-1) );
  rtems_test_assert( n_mq != Test_q[ DEFAULT_RW ].mq );


  status = mq_unlink( DEFAULT_NAME );
  fatal_posix_service_status( status, 0, "mq_unlink locked message queue");
  status = mq_close( Test_q[ DEFAULT_RW ].mq );
  fatal_posix_service_status( status, 0, "mq_close message queue");

  Test_q[ DEFAULT_RW ].mq = n_mq;
}

void verify_close_functionality(void)
{
  int i;
  int status;
  Start_Test( "Unlink and Close All Files"  );
  for (i=0; i<DEFAULT_RW; i++) {

    status = mq_unlink( Get_Queue_Name(i) );
    fatal_posix_service_status( status, 0, "mq_unlink message queue");

    status = mq_close( Test_q[i].mq );
    fatal_posix_service_status( status, 0, "mq_close message queue");
  }
}


void verify_timed_send_queue(
  int  que,
  int  is_blocking
)
{
  struct timespec timeout;
  struct timeval  tv1, tv2, tv3;
  struct timezone tz1, tz2;
  int              len;
  int              status;
  char            *msg;

  printf( "Init: mq_timedsend - on queue %s ", Test_q[que].name);
  len = Predefined_Msgs[MAXMSG].size;
  msg = Predefined_Msgs[MAXMSG].msg;

  gettimeofday( &tv1, &tz1 );
  timeout.tv_sec  = tv1.tv_sec + 1;
  timeout.tv_nsec = tv1.tv_usec * 1000;

  status = mq_timedsend( Test_q[que].mq, msg, len , 0, &timeout );

  gettimeofday( &tv2, &tz2 );
  tv3.tv_sec  = tv2.tv_sec - tv1.tv_sec;
  tv3.tv_usec = tv2.tv_usec - tv1.tv_usec;

  if ( is_blocking ) { /* Don't verify the non-blocking queue */
    fatal_int_service_status( status, -1, "mq_timedsend status" );
    fatal_posix_service_status( errno, ETIMEDOUT,  "errno ETIMEDOUT" );
  }

  printf( "Init: %ld sec %ld us\n", (long)tv3.tv_sec, (long)tv3.tv_usec );

  if ( que == DEFAULT_RW )
    Test_q[que].count++;
}

void verify_timed_send(void)
{
  int              que;

  Start_Test( "mq_timedsend"  );

  for( que = RW_QUEUE; que < CLOSED; que++ ) {
    if ( que == BLOCKING )
      verify_timed_send_queue( que, 1 );
    else
      verify_timed_send_queue( que, 0 );
  }
}

void verify_timed_receive_queue(
  char *task_name,
  int   que,
  int   is_blocking
)
{
  char message[ 100 ];
  unsigned int priority;
  struct timespec tm;
  struct timeval  tv1, tv2, tv3;
  struct timezone tz1, tz2;
  int              status;

  printf(
    "Init: %s mq_timedreceive - on queue %s ",
    task_name,
    Test_q[que].name
  );

  gettimeofday( &tv1, &tz1 );
  tm.tv_sec  = tv1.tv_sec + 1;
  tm.tv_nsec = tv1.tv_usec * 1000;

  status = mq_timedreceive( Test_q[ que ].mq, message, 100, &priority, &tm );

  gettimeofday( &tv2, &tz2 );
  tv3.tv_sec  = tv2.tv_sec - tv1.tv_sec;
  tv3.tv_usec = tv2.tv_usec - tv1.tv_usec;

  fatal_int_service_status( status, -1, "mq_timedreceive status");
  if ( is_blocking )
    fatal_posix_service_status( errno, ETIMEDOUT,  "errno ETIMEDOUT");
  printf( "Init: %ld sec %ld us\n", (long)tv3.tv_sec, (long)tv3.tv_usec );

}

void verify_timed_receive(void)
{
  int  que;

  Start_Test( "mq_timedreceive"  );

  for( que = RW_QUEUE; que < CLOSED; que++ ) {
    if (( que == BLOCKING ) || ( que == DEFAULT_RW ))
      verify_timed_receive_queue( "Init:", que, 1 );
    else
      verify_timed_receive_queue( "Init:", que, 0 );
  }
}

#if (0)
void verify_set_attr(void)
{
  struct mq_attr save_attr[ NUMBER_OF_TEST_QUEUES ];
  struct mq_attr attr;
  int            i;
  int            status;

  attr.mq_maxmsg  = 0;
  attr.mq_msgsize = 0;

  Start_Test( "mq_setattr"  );

  puts( "Init: set_attr all queues to blocking" );
  for(i=0; i<CLOSED; i++) {
    attr.mq_flags =  Test_q[i].oflag & (~O_NONBLOCK );
    status = mq_setattr( Test_q[i].mq, &attr, &save_attr[i] );
    fatal_int_service_status( status, 0, "mq_setattr valid return status");

    Validate_attributes( Test_q[i].mq, attr.mq_flags, 0 );
  }

  for( i = RW_QUEUE; i < CLOSED; i++ ) {
    verify_timed_receive_queue( "Init:", i, 1 );
  }

  for(i=0; i<CLOSED; i++) {
    attr.mq_flags =  Test_q[i].oflag & (~O_NONBLOCK );
    status = mq_setattr( Test_q[i].mq, &save_attr[i], NULL );
    fatal_int_service_status( status, 0, "mq_setattr valid return status");

    Validate_attributes( Test_q[i].mq, Test_q[i].oflag, 0 );
  }
}
#endif

void wait_for_signal(
  sigset_t     *waitset,
  int           sec,
  int           expect_signal
)
{
  siginfo_t         siginfo;
  int               status;
  struct timespec   timeout;
  int               signo;

  siginfo.si_code = -1;
  siginfo.si_signo = -1;
  siginfo.si_value.sival_int = -1;

  timeout.tv_sec = sec;
  timeout.tv_nsec = 0;

  status = sigemptyset( waitset );
  rtems_test_assert( !status );

  status = sigaddset( waitset, SIGUSR1 );
  rtems_test_assert( !status );

  printf( "waiting on any signal for %d seconds.\n", sec );
  signo = sigtimedwait( waitset, &siginfo, &timeout );
  if (expect_signal) {
    fatal_int_service_status( signo, SIGUSR1, "got SISUSR1" );
  } else {
    fatal_int_service_status( signo, -1, "error return status");
    fatal_posix_service_status( errno, EAGAIN, "errno EAGAIN");
  }
}

void verify_notify(void)
{
  struct sigevent event;
  int             status;
  timer_t         timer_id;
  sigset_t        set;

  Start_Test( "mq_notify"  );

  /* timer create */
  event.sigev_notify = SIGEV_SIGNAL;
  event.sigev_signo  = SIGUSR1;
  if (timer_create (CLOCK_REALTIME, &event, &timer_id) == -1)
    fatal_posix_service_status( errno, 0,  "errno ETIMEDOUT");

  /* block the timer signal */
  sigemptyset( &set );
  sigaddset( &set, SIGUSR1 );
  pthread_sigmask( SIG_BLOCK, &set, NULL );

  /*
   * EBADF - Not A Valid Message Queue
   */

  puts( "Init: mq_notify - Unopened message queue (EBADF)" );
  status = mq_notify( Test_q[CLOSED].mq, NULL );
  fatal_posix_service_status( status, -1, "mq_ error return status");
  fatal_posix_service_status( errno, EBADF, "mq_receive errno EBADF");

  /*
   * Create ...
   */

  /*
   * XXX setup notification
   */
  printf( "_____mq_notify - notify when %s gets a message\n",RW_NAME);
  status = mq_notify( Test_q[RW_QUEUE].mq, &event );
  fatal_posix_service_status( status, 0, "mq_notify valid status");
  wait_for_signal( &set, 3, 0 );

  /*
   * Send and verify signal occurs and registration is removed.
   */

  puts( "Init: Verify Signal when send" );
  Show_send_msg_to_que( "Init:", RW_QUEUE, 0 );
  wait_for_signal( &set, 3, 1 );
  Read_msg_from_que( RW_QUEUE, 0 );

  puts( "Init: Verify No Signal when send" );
  Show_send_msg_to_que( "Init:", RW_QUEUE, 0 );
  wait_for_signal( &set, 3, 0 );
  Read_msg_from_que( RW_QUEUE, 0 );

  /*
   * EBUSY - Already Registered
   */

  printf( "____mq_notify - notify when %s gets a message\n",RD_NAME);
  status = mq_notify( Test_q[RW_QUEUE].mq, &event );
  fatal_posix_service_status( status, 0, "mq_notify valid status");
  wait_for_signal( &set, 3, 0 );

  puts( "Init: mq_notify -  (EBUSY)" );
  status = mq_notify( Test_q[RW_QUEUE].mq, &event );
  fatal_posix_service_status( status, -1, "mq_notify error return status");
  fatal_posix_service_status( errno, EBUSY, "mq_notify errno EBUSY");

  /*
   * Verify NULL removes registration.
   */

  puts( "Init: mq_notify - Remove notification with null" );
  status = mq_notify( Test_q[RW_QUEUE].mq, NULL );
  fatal_posix_service_status( status, 0, "mq_notify valid status");

  puts( "Init: Verify No Signal when send" );
  Show_send_msg_to_que( "Init:", RW_QUEUE, 0 );
  wait_for_signal( &set, 3, 0 );
  Read_msg_from_que( RW_QUEUE, 0 );

}

void verify_with_threads(void)
{
  int               status;
  pthread_t         id;
  Test_Message_t   *ptr;
#if 0
  unsigned int      priority;
  char              message[100];
#endif


#if 0
  /*
   * Create a task then block until the task sends the message.
   * Task tests set attributes so one queue will have a thread
   * blocked while attributes are changed.
   */

  Start_Test( "multi-thread Task 4 Receive Test"  );
  status = pthread_create( &id, NULL, Task_4, NULL );
  rtems_test_assert( !status );
  puts( "Init: mq_receive - Empty queue changes to non-blocking (EAGAIN)" );
  status = mq_receive( Test_q[BLOCKING].mq, message, 100, &priority );
  fatal_int_service_status( status, -1, "mq_receive error return status");
  fatal_posix_service_status( errno, EAGAIN, "mq_receive errno EAGAIN");
  print_current_time( "Init: ", "" );
#endif
  /*
   * Create a task then block until the task sends the message.
   * Task tests set attributes so one queue will have a thread
   * blocked while attributes are changed.
   */

  Start_Test( "multi-thread Task 1 Test"  );
  status = pthread_create( &id, NULL, Task_1, NULL );
  rtems_test_assert( !status );
  Read_msg_from_que(  BLOCKING, 0 ); /* Block until init writes */
  print_current_time( "Init: ", "" );

#if 0
  /*
   * Fill the queue then create a task then block until the task receives a message.
   * Task tests set attributes so one queue will have a thread
   * blocked while attributes are changed.
   */

  Start_Test( "multi-thread Task 4 Send Test"  );
  fill_message_queues( "Init:" );
  status = pthread_create( &id, NULL, Task_4, NULL );
  rtems_test_assert( !status );
  puts( "Init: mq_send - Full queue changes to non-blocking (EAGAIN)" );
  status = mq_send(Test_q[BLOCKING].mq, message, 0, 0 );
  fatal_posix_service_status( status, -1, "mq_send error return status");
  fatal_posix_service_status( errno, EAGAIN, "mq_send errno EAGAIN");
  verify_queues_full( "Init:" );
  empty_message_queues( "Init:" );
#endif
  /*
   * Create a task then block until the task reads a message.
   */

  Start_Test( "multi-thread Task 2 Test"  );
  fill_message_queues( "Init:" );
  status = pthread_create( &id, NULL, Task_2, NULL );
  rtems_test_assert( !status );
  Show_send_msg_to_que( "Init:", BLOCKING, Priority_Order[0] );
  print_current_time( "Init: ", "" );
  verify_queues_full( "Init:" );
  empty_message_queues( "Init:" );

  /*
   * Create a task then block until it deletes and closes all queues.
   *     EBADF - Queue unlinked and closed while blocked
   */

  Start_Test( "multi-thread Task 3 Test"  );
  fill_message_queues( "Init:" );
  status = pthread_create( &id, NULL, Task_3, NULL );
  rtems_test_assert( !status );
  puts( "Init: mq_send - Block while thread deletes queue (EBADF)" );
  ptr = &Predefined_Msgs[0];
  status = mq_send( Test_q[BLOCKING].mq, ptr->msg, ptr->size , ptr->priority );
  fatal_posix_service_status( status, -1, "mq_send error return status");
  fatal_posix_service_status( errno, EBADF, "mq_send errno EBADF");

}

void validate_mq_setattr(void)
{
  struct mq_attr  attr;
  struct mq_attr  save_attr[ NUMBER_OF_TEST_QUEUES ];
  int             status;
  int            i;

  /*
   * EBADF - Get the attributes from a closed queue.
   */

  puts( "Task1:mq_setattr - unopened queue (EBADF)" );
  status = mq_setattr( Test_q[CLOSED].mq, &attr, NULL );
  fatal_posix_service_status( status, -1, "mq_setattr error return status");
  fatal_posix_service_status( errno, EBADF, "mq_setattr errno EBADF");

  /*
   * XXX - The following are not listed in the POSIX manual but
   *       may occur.
   */

  /*
   * EINVAL - NULL attributes
   */

  puts( "Task1:mq_setattr - NULL attributes (EINVAL)" );
  status = mq_setattr( Test_q[RW_QUEUE].mq, NULL, NULL );
  fatal_posix_service_status( status, -1, "mq_setattr error return status");
  fatal_posix_service_status( errno, EINVAL, "mq_setattr errno EINVAL");

  /*
   * Verify change queues to blocking, by verifying all queues block
   * for a timed receive.
   */

  puts( "Init: set_attr all queues to blocking" );
  for(i=0; i<CLOSED; i++) {
    attr.mq_flags =  Test_q[i].oflag & (~O_NONBLOCK );
    status = mq_setattr( Test_q[i].mq, &attr, &save_attr[i] );
    fatal_int_service_status( status, 0, "mq_setattr valid return status");
    Validate_attributes( Test_q[i].mq, attr.mq_flags, 0 );
  }
  for( i = RW_QUEUE; i < CLOSED; i++ ) {
    verify_timed_receive_queue( "Init:", i, 1 );
  }

  /*
   * Restore restore all queues to their old attribute.
   */

  for(i=0; i<CLOSED; i++) {
    status = mq_setattr( Test_q[i].mq, &save_attr[i], NULL );
    fatal_int_service_status( status, 0, "mq_setattr valid return status");
    Validate_attributes( Test_q[i].mq, Test_q[i].oflag, 0 );
  }
}

void verify_timedout_mq_timedreceive(
  char *task_name,
  int   que,
  int   is_blocking
)
{
  char             message[ 100 ];
  struct timespec  tm;
  struct timeval   tv1, tv2, tv3;
  struct timezone  tz1, tz2;
  int              status;

  printf(
    "Init: %s verify_timedout_mq_timedreceive - on queue %s ",
    task_name,
    Test_q[que].name
  );

  gettimeofday( &tv1, &tz1 );
  tm.tv_sec  = tv1.tv_sec - 1;
  tm.tv_nsec = tv1.tv_usec * 1000;

  status = mq_timedreceive( Test_q[ que ].mq, message, 100, NULL, &tm );

  gettimeofday( &tv2, &tz2 );
  tv3.tv_sec  = tv2.tv_sec - tv1.tv_sec;
  tv3.tv_usec = tv2.tv_usec - tv1.tv_usec;

  fatal_int_service_status( status, -1, "mq_timedreceive status");

/* FIXME: This is wrong. */
  printf( "Init: %ld sec %ld us\n", (long)tv3.tv_sec, (long)tv3.tv_usec );
}

void verify_mq_receive(void)
{
  int  que;

  Start_Test( "mq_timedout_receive"  );

  for( que = RW_QUEUE; que < CLOSED; que++ ) {
    if (( que == BLOCKING ) || ( que == DEFAULT_RW ))
      break;
    else
      verify_timedout_mq_timedreceive( "Init:", que, 0 );
  }
}

void verify_timedout_mq_timedsend(
  int  que,
  int  is_blocking
)
{
  struct timespec timeout;
  struct timeval  tv1, tv2, tv3;
  struct timezone tz1, tz2;
  int              len;
  char            *msg;

  printf( "Init: verify_timedout_mq_timedsend - on queue %s ", Test_q[que].name);
  len = Predefined_Msgs[MAXMSG].size;
  msg = Predefined_Msgs[MAXMSG].msg;

  gettimeofday( &tv1, &tz1 );
  timeout.tv_sec  = tv1.tv_sec - 1;
  timeout.tv_nsec = tv1.tv_usec * 1000;

  (void) mq_timedsend( Test_q[que].mq, msg, len , 0, &timeout );

  gettimeofday( &tv2, &tz2 );
  tv3.tv_sec  = tv2.tv_sec - tv1.tv_sec;
  tv3.tv_usec = tv2.tv_usec - tv1.tv_usec;

  printf( "Init: %ld sec %ld us\n", (long)tv3.tv_sec, (long)tv3.tv_usec );

  if ( que == DEFAULT_RW )
    Test_q[que].count++;
}

void verify_mq_send(void)
{
  int              que;

  Start_Test( "verify_timedout_mq_timedsend"  );

  for( que = RW_QUEUE; que < CLOSED; que++ ) {
    if ( que == BLOCKING )
      verify_timedout_mq_timedsend( que, 1 );
    else
      verify_timedout_mq_timedsend( que, 0 );
  }
}

void *POSIX_Init(
  void *argument
)
{
  puts( "\n\n*** POSIX MESSAGE QUEUE TEST ***" );

  validate_mq_open_error_codes( );
  open_test_queues();
  validate_mq_unlink_error_codes();
  validate_mq_close_error_codes();
  verify_unlink_functionality();
  validate_mq_setattr( );
  validate_mq_send_error_codes();
  validate_mq_getattr_error_codes();
  verify_timed_send();
  validate_mq_receive_error_codes();
  verify_timed_receive();
  verify_open_functionality();
  verify_notify();
  verify_with_threads();
  verify_mq_receive();
  verify_mq_send();

  puts( "*** END OF POSIX MESSAGE QUEUE TEST ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}


void *Task_1 (
  void *argument
)
{
  /* Block Waiting for a message */

  print_current_time( "Task_1: ", "" );

  Show_send_msg_to_que( "Task_1:", BLOCKING, 0 );

  puts( "Task_1: pthread_exit" );
  pthread_exit( NULL );

  /* switch to Init */

  rtems_test_assert( 0 );
  return NULL; /* just so the compiler thinks we returned something */
}

void *Task_2(
  void *argument
)
{
  print_current_time( "Task_2: ", "" );


  /* Block waiting to send a message */

  verify_queues_full( "Task_2:" );
  Read_msg_from_que( BLOCKING, Priority_Order[0] ); /* Cause context switch */

  puts( "Task_2: pthread_exit" );
  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */
}

void *Task_3 (
  void *argument
)
{

  print_current_time( "Task_3: ", "" );

  /*
   * close and unlink all queues.
   */

  verify_close_functionality();
  puts( "Task_3: pthread_exit" );
  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */

}

void *Task_4 (
  void *argument
)
{
  struct mq_attr  attr;
  int             status;
  int             count;

  print_current_time( "Task_4: ", "" );

  /*
   * Set the count to the number of messages in the queue.
   */

  status = mq_getattr( Test_q[BLOCKING].mq, &attr );
  fatal_posix_service_status( status, 0, "mq_getattr valid return status");
  count = attr.mq_curmsgs;

  puts("Task_4: Set queue to non-blocking");
  attr.mq_flags =  Test_q[BLOCKING].oflag | O_NONBLOCK;
  status = mq_setattr( Test_q[BLOCKING].mq, &attr, NULL );
  fatal_int_service_status( status, 0, "mq_setattr valid return status");
  Validate_attributes( Test_q[BLOCKING].mq, attr.mq_flags, count );

  puts("Task_4: Return queue to blocking");
  attr.mq_flags =  Test_q[BLOCKING].oflag;
  status = mq_setattr( Test_q[BLOCKING].mq, &attr, NULL );
  fatal_int_service_status( status, 0, "mq_setattr valid return status");
  Validate_attributes( Test_q[BLOCKING].mq, attr.mq_flags, count );

  puts( "Task_4: pthread_exit" );
  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */

}

void *Task_5 (
  void *argument
)
{

  print_current_time( "Task_5: ", "" );

  puts( "Task_5: pthread_exit" );
  pthread_exit( NULL );

     /* switch to Init */

  return NULL; /* just so the compiler thinks we returned something */

}
