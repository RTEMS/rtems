/*
 *  Classic API Init task create failure
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*
 *  Way too much stack space.  Should generate a fatal error
 *  on the init task create.
 */
#define CONFIGURE_HAS_OWN_INIT_TASK_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE   RTEMS_MINIMUM_STACK_SIZE
rtems_initialization_tasks_table Initialization_tasks[] = {
  { rtems_build_name('I', 'N', 'I', ' '),
    RTEMS_MINIMUM_STACK_SIZE,
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    Init,
    RTEMS_DEFAULT_MODES,
    0
  }
};
#define CONFIGURE_INIT_TASK_TABLE Initialization_tasks
#define CONFIGURE_INIT_TASK_TABLE_SIZE \
  sizeof(CONFIGURE_INIT_TASK_TABLE) / sizeof(rtems_initialization_tasks_table)

#define SPFATAL_TEST_CASE_EXTRA_TASKS 1

#define FATAL_ERROR_TEST_NAME            "5"
#define FATAL_ERROR_DESCRIPTION          "Core fall out of a task"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_IS_INTERNAL TRUE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_THREAD_EXITTED

rtems_task EmptyTask( rtems_task_argument unused );

rtems_task EmptyTask(
  rtems_task_argument unused
)
{
}

void force_error()
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
