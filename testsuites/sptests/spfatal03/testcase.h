/*
 *  Semaphore Obtain in Critical Section
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/bspIo.h>

#define CONFIGURE_MAXIMUM_SEMAPHORES 10

#define FATAL_ERROR_TEST_NAME            "3"
#define FATAL_ERROR_DESCRIPTION          "Core Mutex obtain in critical section"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       \
          INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL

void force_error(void)
{

  rtems_status_code status;
   rtems_id    mutex;

  status = rtems_semaphore_create(
    rtems_build_name( 'S','0',' ',' '),
    0,
    RTEMS_LOCAL|
    RTEMS_SIMPLE_BINARY_SEMAPHORE,
    0,
    &mutex
  );
  directive_failed( status, "rtems_semaphore_create of S0");
  printk("Create semaphore S0\n");

  printk("Obtain semaphore in dispatching critical section\n");
  _Thread_Dispatch_disable();
  status = rtems_semaphore_obtain( mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  /* !!! SHOULD NOT RETURN FROM THE ABOVE CALL */

  rtems_test_assert( 0 );
  /* we will not run this far */
}
