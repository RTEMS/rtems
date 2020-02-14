#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 *  Classic API Init task create failure
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define SPFATAL_TEST_CASE_EXTRA_TASKS 1

#define FATAL_ERROR_TEST_NAME            "5"
#define FATAL_ERROR_DESCRIPTION          "Core fall out of a task"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_THREAD_EXITTED

rtems_task EmptyTask( rtems_task_argument unused );

rtems_task EmptyTask(
  rtems_task_argument unused
)
{
}

static void force_error(void)
{
  rtems_id          id;
  rtems_status_code status;

  status = rtems_task_create(
    rtems_build_name('T','A','0','1' ),
    4,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( id, EmptyTask, 0 );
  directive_failed( status, "rtems_task_start" );

  status = rtems_task_wake_after( 100 );
  directive_failed( status, "rtems_task_wake_after" );

  /* we will not run this far */
}

#include "../spfatal_support/spfatalimpl.h"
