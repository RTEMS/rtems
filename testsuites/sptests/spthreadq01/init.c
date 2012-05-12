/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

#include <sys/types.h>
#include <rtems/score/threadq.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void threadq_first_empty(
  const char               *discipline_string,
  Thread_queue_Disciplines  discipline
);

void threadq_first_empty(
  const char               *discipline_string,
  Thread_queue_Disciplines  discipline
)
{
  Thread_queue_Control tq;

  printf( "Init - initialize thread queue for %s\n", discipline_string );
  _Thread_queue_Initialize( &tq, discipline, 0x01, 3 );

  puts( "Init - _Thread_queue_Extract - thread not blocked on a thread queue" );
  _Thread_queue_Extract( &tq, _Thread_Executing );
  /* is there anything to check? */
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  puts( "\n\n*** TEST THREAD QUEUE TEST 01 ***" );

  threadq_first_empty( "FIFO", THREAD_QUEUE_DISCIPLINE_FIFO );
  threadq_first_empty( "Priority", THREAD_QUEUE_DISCIPLINE_PRIORITY );

  puts( "*** END OF TEST THREAD QUEUE TEST 01 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS  1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
