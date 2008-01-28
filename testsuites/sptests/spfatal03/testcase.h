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
#define CONFIGURE_INIT_TASK_STACK_SIZE            RTEMS_MINIMUM_STACK_SIZE 
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

#define CONFIGURE_MAXIMUM_SEMAPHORES 10

#define FATAL_ERROR_DESCRIPTION          "Core Mutex obtain in critical section"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_IS_INTERNAL FALSE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_MUTEX_OBTAIN_FROM_BAD_STATE 

rtems_id   Mutex_id[1];
rtems_name Mutex_name[1];

void force_error()
{
  ISR_Level level;
  rtems_status_code status;


  Mutex_name[0] = rtems_build_name( 'S','0',' ',' ');
  status = rtems_semaphore_create(
    Mutex_name[0],
    1,
    RTEMS_LOCAL|
    RTEMS_SIMPLE_BINARY_SEMAPHORE|
    RTEMS_PRIORITY_CEILING |
    RTEMS_PRIORITY,
    0,
    &Mutex_id[0]
  );
  directive_failed( status, "rtems_semaphore_create of S0");
  printf("Create S0\n");


  _Thread_Disable_dispatch();
  status = rtems_semaphore_obtain( Mutex_id[0], RTEMS_DEFAULT_OPTIONS, 0 );
  /* !!! SHOULD NOT RETURN FROM THE ABOVE CALL */

  _Thread_Enable_dispatch();
  _ISR_Enable( level );
  directive_failed( status, "rtems_semaphore_obtain" );
 
  /* we will not run this far */
}
