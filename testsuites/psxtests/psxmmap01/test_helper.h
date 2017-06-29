/*
 * Copyright (c) 2017 Kevin Kirspel.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <string.h>

#define mmap_test_assert(__exp, fmt, ...) \
  do { \
    if (!(__exp)) { \
      fail_check(__FILE__, __LINE__, fmt, ##__VA_ARGS__); \
      rtems_test_exit(0); \
    } \
  } while (0)

#define mmap_test_check(__exp, fmt, ...) \
  do { \
    if (!(__exp)) { \
      fail_check(__FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } else { \
      rtems_test_assert(__exp); \
    }\
  } while (0)

#define mmap_test_assert_equal(expected, actual, fmt, ...) \
    mmap_test_assert((expected) == (actual), "%s != %s: " fmt, \
                  #expected, #actual, ##__VA_ARGS__)

#define mmap_test_check_equal(expected, actual, fmt, ...) \
    mmap_test_check((expected) == (actual), "%s != %s: " fmt, \
                  #expected, #actual, ##__VA_ARGS__)

extern void fail_check(
  const char *file,
  const size_t line,
  const char *fmt,
  ...
);
