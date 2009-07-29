/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <pmacros.h>
#include <unistd.h>
#include <errno.h>
#include <tmacros.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#include "test_support.h"

void *POSIX_Init(
  void *argument
)
{
  struct mq_attr   attr;
  mqd_t            Queue;
  int              sc;

  puts( "\n\n*** POSIX MESSAGE QUEUE TEST 4 ***" );

  attr.mq_maxmsg  = 1;
  attr.mq_msgsize = sizeof(int);

  puts( "Init - Open message queue" );
  Queue = mq_open( "Queue", O_CREAT | O_RDWR, 0x777, &attr );
  if ( Queue == (-1) )
    perror( "mq_open failed" );
  assert( Queue != (-1) );

  puts( "Init - Unlink message queue" );
  sc = mq_unlink( "Queue" );
  if ( sc != 0 )
    perror( "mq_unlink failed" );
  assert( sc == 0 );

  puts( "Init - Close message queue" );
  sc = mq_close( Queue );
  if ( sc != 0 )
    perror( "mq_close failed" );
  assert( sc == 0 );

  puts( "*** END OF POSIX MESSAGE QUEUE TEST 4 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_THREADS        1
#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of include file */
