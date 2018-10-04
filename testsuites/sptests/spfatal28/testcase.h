/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#define FATAL_ERROR_TEST_NAME            "28"
#define FATAL_ERROR_DESCRIPTION          "delete a task with a semaphore in use"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_RESOURCE_IN_USE

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

void force_error()
{
#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
  rtems_status_code sc;
  rtems_id id;

  sc = rtems_semaphore_create(
    rtems_build_name('S', 'E', 'M', 'A'),
    0,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    0,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_task_exit();
#else
  _Terminate( INTERNAL_ERROR_CORE, INTERNAL_ERROR_RESOURCE_IN_USE );
#endif
}
