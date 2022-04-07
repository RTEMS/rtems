/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 Kevin Kirspel.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
