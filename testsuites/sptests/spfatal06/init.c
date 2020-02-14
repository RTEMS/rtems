#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 *  Classic API Init task create failure
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define FATAL_ERROR_TEST_NAME            "6"
#define FATAL_ERROR_DESCRIPTION \
        "Core initialize with invalid stack hook"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR       INTERNAL_ERROR_BAD_STACK_HOOK

#define CONFIGURE_TASK_STACK_ALLOCATOR New_stack_allocate_hook

#define CONFIGURE_TASK_STACK_DEALLOCATOR NULL

static void *New_stack_allocate_hook(size_t unused)
{
  return NULL;
}

static void force_error(void)
{
  /* we will not run this far */
}

#include "../spfatal_support/spfatalimpl.h"
