/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"

#include <rtems.h>
#include <unistd.h>
#include <assert.h>

const char rtems_test_name[] = "GETENTROPY 1";

#define TEST_MAX_BYTES 32
#define TEST_GUARD_BYTES 32
#define TEST_GUARD 0xA5

static char entropy_buffer1 [TEST_MAX_BYTES + TEST_GUARD_BYTES];
static char entropy_buffer2 [TEST_MAX_BYTES + TEST_GUARD_BYTES];
static char guard_buffer [TEST_MAX_BYTES + TEST_GUARD_BYTES];

static void test_getentropy(size_t nr_bytes)
{
  int rv;
  size_t try = 1;

  /*
   * For very small patterns, the probability that two random patterns are the
   * same is quite high. Therefore retry the test for these a few times.
   */
  if (nr_bytes < 4) {
    try = 4;
  }

  do {
    --try;

    /* Fill buffers with guard pattern */
    memset(entropy_buffer1, TEST_GUARD, sizeof(entropy_buffer1));
    memset(entropy_buffer2, TEST_GUARD, sizeof(entropy_buffer2));
    memset(guard_buffer, TEST_GUARD, sizeof(entropy_buffer2));

    /* Get entropy with the given size */
    rv = getentropy(entropy_buffer1, nr_bytes);
    assert(rv == 0);
    rv = getentropy(entropy_buffer2, nr_bytes);
    assert(rv == 0);

    /* Check guard pattern */
    rv = memcmp(entropy_buffer1 + nr_bytes, guard_buffer + nr_bytes,
        sizeof(entropy_buffer1) - nr_bytes);
    assert(rv == 0);
    rv = memcmp(entropy_buffer2 + nr_bytes, guard_buffer + nr_bytes,
        sizeof(entropy_buffer2) - nr_bytes);
    assert(rv == 0);

    /* Make sure that buffers are not the same */
    rv = memcmp(entropy_buffer1, entropy_buffer2, sizeof(entropy_buffer1));
  } while (try > 0 && rv == 0);
  assert(rv != 0);
}

static void Init(rtems_task_argument arg)
{
  size_t i;

  TEST_BEGIN();

  for (i = 1; i <= TEST_MAX_BYTES; ++i) {
    test_getentropy(i);
  }

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
