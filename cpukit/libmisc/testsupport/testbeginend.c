/*
 * Copyright (c) 2014, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * Copyright (c) 2017 Chris Johns <chrisj@rtems.org>. All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/test.h>
#include <rtems/bspIo.h>
#include <rtems/version.h>

#define TEST_BUILD_DEFAULT "default"
#if RTEMS_POSIX
  #define TEST_BUILD_DEFAULT ""
  #define TEST_BUILD_POSIX   "posix "
#else
  #define TEST_BUILD_POSIX
#endif
#if RTEMS_SMP
  #define TEST_BUILD_DEFAULT ""
  #define TEST_BUILD_SMP     "smp "
#else
  #define TEST_BUILD_SMP
#endif
#if RTEMS_MULTIPROCESSING
  #define TEST_BUILD_DEFAULT ""
  #define TEST_BUILD_MP      "mp "
#else
  #define TEST_BUILD_MP
#endif
#if RTEMS_PARAVIRT
  #define TEST_BUILD_DEFAULT  ""
  #define TEST_BUILD_PARAVIRT "paravirt "
#else
  #define TEST_BUILD_PARAVIRT
#endif
#if RTEMS_NETWORKING
  #define TEST_BUILD_DEFAULT    ""
  #define TEST_BUILD_NETWORKING "legacy-net "
#else
  #define TEST_BUILD_NETWORKING
#endif
#if RTEMS_DEBUG
  #define TEST_BUILD_DEFAULT ""
  #define TEST_BUILD_DEBUG   "debug "
#else
  #define TEST_BUILD_DEBUG
#endif
#if RTEMS_PROFILING
  #define TEST_BUILD_DEFAULT   ""
  #define TEST_BUILD_PROFILING "profiling "
#else
  #define TEST_BUILD_PROFILING
#endif

#define TEST_BUILD_STRING \
         TEST_BUILD_DEFAULT \
         TEST_BUILD_POSIX \
         TEST_BUILD_SMP \
         TEST_BUILD_MP \
         TEST_BUILD_PARAVIRT \
         TEST_BUILD_NETWORKING \
         TEST_BUILD_DEBUG \
         TEST_BUILD_PROFILING

rtems_printer rtems_test_printer = {
  .printer = rtems_printk_printer
};

static const char* const test_state_strings[] =
{
  "EXPECTED-PASS",
  "EXPECTED-FAIL",
  "USER_INPUT",
  "INDETERMINATE",
  "BENCHMARK"
};

int rtems_test_begin(const char* name, const RTEMS_TEST_STATE state)
{
  int l;
  l = rtems_printf(
    &rtems_test_printer,
    "\n\n*** BEGIN OF TEST %s ***\n", name
  );
  l += rtems_printf(
    &rtems_test_printer,
    "*** TEST VERSION: %s\n", rtems_version()
  );
  l += rtems_printf(
    &rtems_test_printer,
    "*** TEST STATE: %s\n", test_state_strings[state]
  );
  l += rtems_printf(
    &rtems_test_printer,
    "*** TEST BUILD: %s\n", TEST_BUILD_STRING
  );
  l += rtems_printf(
    &rtems_test_printer,
    "*** TEST TOOLS: " __VERSION__ "\n"
  );
  return l;
}

int rtems_test_end(const char* name)
{
  return rtems_printf(
    &rtems_test_printer,
    "\n*** END OF TEST %s ***\n\n", name
  );
}

void rtems_test_exit(int status)
{
  (void) status;
  rtems_shutdown_executive(0);
}

int rtems_test_printf(
  const char* format,
  ...
)
{
  va_list ap;
  int len;
  va_start(ap, format);
  len = rtems_vprintf(
    &rtems_test_printer,
    format,
    ap
  );
  va_end(ap);
  return len;
}
