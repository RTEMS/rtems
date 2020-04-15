/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
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

rtems_printer rtems_test_printer = {
  .printer = rtems_printk_printer
};

static const char* const test_state_strings[] =
{
  "EXPECTED_PASS",
  "EXPECTED_FAIL",
  "USER_INPUT",
  "INDETERMINATE",
  "BENCHMARK"
};

int rtems_test_begin(const char* name, const RTEMS_TEST_STATE state)
{
  return rtems_printf(
    &rtems_test_printer,
    "\n\n*** BEGIN OF TEST %s ***\n"
    "*** TEST VERSION: %s\n"
    "*** TEST STATE: %s\n"
    "*** TEST BUILD:"
#if RTEMS_DEBUG
    " RTEMS_DEBUG"
#endif
#if RTEMS_MULTIPROCESSING
    " RTEMS_MULTIPROCESSING"
#endif
#if RTEMS_NETWORKING
    " RTEMS_NETWORKING"
#endif
#if RTEMS_PARAVIRT
    " RTEMS_PARAVIRT"
#endif
#if RTEMS_POSIX_API
    " RTEMS_POSIX_API"
#endif
#if RTEMS_PROFILING
    " RTEMS_PROFILING"
#endif
#if RTEMS_SMP
    " RTEMS_SMP"
#endif
    "\n"
    "*** TEST TOOLS: " __VERSION__ "\n",
    name,
    rtems_version(),
    test_state_strings[state]
  );
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
