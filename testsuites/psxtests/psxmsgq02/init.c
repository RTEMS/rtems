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
#include <errno.h>
#include "tmacros.h"
#include <mqueue.h>
#include <sched.h>
#include <fcntl.h>
#include <time.h>
#include <tmacros.h>
#include <signal.h>   /* signal facilities */
#include "test_support.h"

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

pthread_t        Init_id;

void *POSIX_Init(
  void *argument
)
{
  struct mq_attr         attr;
  mqd_t                  mq;

  puts( "\n\n*** POSIX MESSAGE QUEUE 02 TEST ***" );


  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  rtems_workspace_greedy_allocate( NULL, 0 );

  attr.mq_maxmsg  = MAXMSG;
  attr.mq_msgsize = MSGSIZE;
  puts("Init: mq_open - Workspace not available - ENOMEM");
  mq = mq_open( Get_Longest_Name(), O_CREAT, 0x777, &attr );
  fatal_posix_service_status_errno(mq, ENOMEM, "no workspace available");

  puts( "*** END OF POSIX MESSAGE QUEUE 02 TEST ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
