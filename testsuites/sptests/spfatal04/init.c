#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 * Classic API Init task create failure
 */

#define FATAL_ERROR_TEST_NAME            "4"
#define FATAL_ERROR_DESCRIPTION \
        "Classic API call rtems_fatal_error_occurred"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_RTEMS_API
#define FATAL_ERROR_EXPECTED_ERROR       0xdead

static void force_error(void)
{
  rtems_fatal_error_occurred( 0xdead );
  /* we will not run this far */
}

#include "../spfatal_support/spfatalimpl.h"
