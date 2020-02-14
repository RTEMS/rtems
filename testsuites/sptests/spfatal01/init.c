#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 * Classic API Init task create failure
 */

#define FATAL_ERROR_TEST_NAME            "1"
#define FATAL_ERROR_DESCRIPTION          "Classic API Init task create failure"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_RTEMS_INIT_TASK_CREATE_FAILED

static void *stack_allocator(size_t unused)
{
  return NULL;
}

static void stack_deallocator(void *unused)
{
}

#define CONFIGURE_TASK_STACK_ALLOCATOR stack_allocator

#define CONFIGURE_TASK_STACK_DEALLOCATOR stack_deallocator

static void force_error(void)
{
/*
 *  Case 2: Null entry
 *  Case 3: semaphore_create
 *          _Thread_Dispatch_disable
 *          semaphore_obtain
 */

  /* we will not run this far */
}

#include "../spfatal_support/spfatalimpl.h"
