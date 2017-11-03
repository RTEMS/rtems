/*
 * Copyright (C) 2015 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <tmacros.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXCLOCK01";
#include <time.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(void *argument)
{
  clock_t start, end;
  int diff = 0;

  TEST_BEGIN();

  start = clock();
  end = 0;
  if ( start >= 0 ) {
    do {
      end = clock();
      diff = (int)(end - start)/CLOCKS_PER_SEC;
    } while (diff < 1);
  }

  printf("Spun for %d second\n", diff);

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION
#define CONFIGURE_MAXIMUM_POSIX_THREADS     2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
