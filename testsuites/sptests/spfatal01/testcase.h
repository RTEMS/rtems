/*
 * Classic API Init task create failure
 */

/*
 *  Way too much stack space.  Should generate a fatal error
 *  on the init task create.
 */
#define CONFIGURE_HAS_OWN_INIT_TASK_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE 0
rtems_initialization_tasks_table Initialization_tasks[] = {
  { rtems_build_name('I', 'N', 'I', ' '),
    32UL * 1024UL,
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

#define FATAL_ERROR_TEST_NAME            "1"
#define FATAL_ERROR_DESCRIPTION          "Classic API Init task create failure"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_RTEMS_API
#define FATAL_ERROR_EXPECTED_IS_INTERNAL TRUE
#define FATAL_ERROR_EXPECTED_ERROR       RTEMS_UNSATISFIED

void force_error()
{
/*
 *  Case 2: Null entry
 *  Case 3: semaphore_create
 *          _Thread_Disable_dispatch
 *          semaphore_obtain
 */

  /* we will not run this far */
}
