/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief This source file contains the implementation of
 *   rtems_test_begin() and rtems_test_end().
 */

/*
 * Copyright (C) 2014, 2018 embedded brains GmbH & Co. KG
 *
 * Copyright (c) 2017 Chris Johns <chrisj@rtems.org>. All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/test-info.h>
#include <rtems/test-printer.h>
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
