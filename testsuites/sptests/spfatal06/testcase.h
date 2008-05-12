/*
 * Classic API Init task create failure
 *
 *  $Id$
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

#define FATAL_ERROR_DESCRIPTION          "Core initialize with invalid stack hook"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_IS_INTERNAL TRUE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_BAD_STACK_HOOK

void *New_stack_allocate_hook( uint32_t unused)
{
}

void force_error()
{
  rtems_configuration_table New_Configuration;

  New_Configuration = *_Configuration_Table;

  if (_Configuration_Table->stack_free_hook != NULL)
    New_Configuration.stack_allocate_hook = NULL;
  else
     New_Configuration.stack_allocate_hook = &New_stack_allocate_hook;

  rtems_initialize_data_structures( &New_Configuration );
  /* we will not run this far */
}

