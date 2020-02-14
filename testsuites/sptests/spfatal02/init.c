#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../spfatal_support/spfatal.h"

/*
 * Classic API Init task create failure
 */

#define CONFIGURE_INIT_TASK_ENTRY_POINT NULL

#define FATAL_ERROR_TEST_NAME            "2"
#define FATAL_ERROR_DESCRIPTION          "Classic API Init task start failure"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_CORE
#define FATAL_ERROR_EXPECTED_ERROR \
  INTERNAL_ERROR_RTEMS_INIT_TASK_ENTRY_IS_NULL

static void force_error(void)
{
/*
 *  Case 2: Null entry
 */

  /* we will not run this far */
}

#include "../spfatal_support/spfatalimpl.h"
