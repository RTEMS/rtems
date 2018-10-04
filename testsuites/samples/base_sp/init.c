/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include "tmacros.h"
#include <stdio.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

const char rtems_test_name[] = "SAMPLE SINGLE PROCESSOR APPLICATION";

#define ARGUMENT 0

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_name        task_name;
  rtems_id          tid;
  rtems_status_code status;

  TEST_BEGIN();
  printf( "Creating and starting an application task\n" );

  task_name = rtems_build_name( 'T', 'A', '1', ' ' );

  status = rtems_task_create( task_name, 1, RTEMS_MINIMUM_STACK_SIZE,
             RTEMS_INTERRUPT_LEVEL(0), RTEMS_DEFAULT_ATTRIBUTES, &tid );
  directive_failed( status, "create" ); 

  status = rtems_task_start( tid, Application_task, ARGUMENT );
  directive_failed( status, "start" ); 

  rtems_task_exit();
}
