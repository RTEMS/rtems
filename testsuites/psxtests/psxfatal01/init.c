#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../psxfatal_support/psxfatal.h"

/*
 * Classic API Init task create failure
 */

#define CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT NULL

#define FATAL_ERROR_TEST_NAME            "1"
#define FATAL_ERROR_DESCRIPTION \
        "POSIX API Init thread create failure - NULL entry"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR \
  INTERNAL_ERROR_POSIX_INIT_THREAD_CREATE_FAILED

static void force_error(void)
{
  /* we will not run this far */
}

#include "../psxfatal_support/psxfatalimpl.h"
