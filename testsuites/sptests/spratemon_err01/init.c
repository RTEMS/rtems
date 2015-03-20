/*
 *  COPYRIGHT (c) 2014.
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

const char rtems_test_name[] = "SP RATE MONOTONIC ERROR 01";

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  TEST_BEGIN();
  Task_name[ 4 ]       =  rtems_build_name( 'T', 'A', '4', ' ' );
  Period_name[ 1 ]     =  rtems_build_name( 'T', 'M', '1', ' ' );
  
  status = rtems_task_create(
    Task_name[ 4 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 4 ]
  );
  directive_failed( status, "rtems_task_create of TA4" );
  puts( "TA1 - rtems_task_create - 4 created - RTEMS_SUCCESSFUL" );

  Screen10(); 
  TEST_END();
}
