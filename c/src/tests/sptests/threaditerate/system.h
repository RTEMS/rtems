/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

/* RTEMS API */

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>

unsigned32 print_thread_info( Thread_Control *thread, void * arg );

#include <tmacros.h>

/* functions */

rtems_task Init( rtems_task_argument argument );
rtems_task RTEMS_task_1_through_3( rtems_task_argument argument );

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 4 ];         /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];       /* array of task names */

/* configuration information */

#define CONFIGURE_SPTEST

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (4 * RTEMS_MINIMUM_STACK_SIZE)



/* POSIX API */

#ifdef RTEMS_POSIX_API

#include <pmacros.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

/* functions */

void *pthread_1_through_3( void *argument );

/* global variables */

TEST_EXTERN pthread_t pthread_id[3];        /* array of pthread ids */

/* configuration information */

#define CONFIGURE_MAXIMUM_POSIX_THREADS     3

#endif



/* ITRON API */

#ifdef RTEMS_ITRON_API

#include <itron.h>
#include <rtems/itron/task.h>

/* functions */

void ITRON_task_2_through_4( void );

/* global variables */

TEST_EXTERN Objects_Locations location;


/* configuration information */

#endif

#include <confdefs.h>

/* end of include file */
