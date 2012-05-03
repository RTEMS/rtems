/*
 * Classic API Init task create failure
 */

#include <errno.h>

/*
 *  Way too much stack space.  Should generate a fatal error
 *  on the init task create.
 */
#define CONFIGURE_POSIX_HAS_OWN_INIT_THREAD_TABLE
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE 0
posix_initialization_threads_table POSIX_Initialization_threads[] = {
  { POSIX_Init,
#if INT_MAX < (12 * 1024 * 1024)
    INT_MAX
#else
    12 * 1024 * 1024 /* too much stack */
#endif
  }
};

#define CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME POSIX_Initialization_threads

#define CONFIGURE_POSIX_INIT_THREAD_TABLE_SIZE \
  sizeof(CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME) / \
      sizeof(posix_initialization_threads_table)

#define FATAL_ERROR_TEST_NAME            "2"
#define FATAL_ERROR_DESCRIPTION \
        "POSIX API Init thread create failure -- no memory for stack"
#define FATAL_ERROR_EXPECTED_SOURCE      INTERNAL_ERROR_POSIX_API
#define FATAL_ERROR_EXPECTED_IS_INTERNAL TRUE
#define FATAL_ERROR_EXPECTED_ERROR       EAGAIN

void force_error(void)
{
  /* we will not run this far */
}
