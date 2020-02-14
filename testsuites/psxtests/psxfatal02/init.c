#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../psxfatal_support/psxfatal.h"

/*
 * Classic API Init task create failure
 */

#define FATAL_ERROR_TEST_NAME            "2"
#define FATAL_ERROR_DESCRIPTION \
        "POSIX API Init thread create failure -- no memory for stack"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_POSIX_INIT_THREAD_CREATE_FAILED

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
  /* we will not run this far */
}

#include "../psxfatal_support/psxfatalimpl.h"
