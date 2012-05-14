/*
 *  Semaphore Obtain in Critical Section
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_SEMAPHORES 10

#define FATAL_ERROR_TEST_NAME            "3"
#define FATAL_ERROR_DESCRIPTION          "Core Mutex obtain in critical section"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_IS_INTERNAL FALSE
#define FATAL_ERROR_EXPECTED_ERROR       \
          INTERNAL_ERROR_MUTEX_OBTAIN_FROM_BAD_STATE

void force_error(void)
{

  rtems_status_code status;
   rtems_id    mutex;

  status = rtems_semaphore_create(
    rtems_build_name( 'S','0',' ',' '),
    1,
    RTEMS_LOCAL|
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &mutex
  );
  directive_failed( status, "rtems_semaphore_create of S0");
  printk("Create semaphore S0\n");

  printk("Obtain semaphore in dispatching critical section\n");
  _Thread_Disable_dispatch();
  status = rtems_semaphore_obtain( mutex, RTEMS_DEFAULT_OPTIONS, 0 );
  /* !!! SHOULD NOT RETURN FROM THE ABOVE CALL */

  _Thread_Enable_dispatch();
  printk("ERROR -- Obtain semaphore should not have returned\n");

  /* we will not run this far */
}
