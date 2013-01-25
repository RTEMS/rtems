/*
 * Copyright (c) 2012 Deng Hengyi.
 *
 *  This test case is to test atomic store operation.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  int                i;
  char               ch;
  rtems_id           id;
  rtems_status_code  status;
  bool               allDone;

  /* Put start of test message */
  puts( "\n\n***  spatomic02 TEST ***\n" );

  /* Initialize the TaskRan array */
  for ( i=0; i<TASK_NUMS ; i++ ) {
    TaskRan[i] = false;
  }

  /* Create and start tasks for each processor */
  for ( i=0; i< TASK_NUMS ; i++ ) {
    ch = '0' + i;

    status = rtems_task_create(
      rtems_build_name( 'T', 'A', ch, ' ' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "task create" );

    status = rtems_task_start( id, Test_task, i );
    directive_failed( status, "task start" );
  }

  status = rtems_task_create(
      rtems_build_name( 'A', 'T', '0', ' ' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
  directive_failed( status, "task create" );

  status = rtems_task_start( id, Wait_task, 0 );
  directive_failed( status, "task start" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "delete" );
}
