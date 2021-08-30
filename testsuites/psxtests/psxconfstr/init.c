/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *  @brief Test suite for confstr method
 */

/*
 * Copyright (C) 2020 Eshan Dhawan
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

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <rtems/test.h>
#include <rtems/test-info.h>

const char rtems_test_name[] = "PSXCONFSTR";

T_TEST_CASE(confstr)
{

  int r;
  const char UPE[] = "unsupported programming environment";
  char buf[sizeof(UPE)];
  size_t len1;

  len1 = sizeof(UPE);

  r = confstr(_CS_POSIX_V6_ILP32_OFFBIG_CFLAGS, buf, sizeof(buf));
  T_eq_sz(r, len1);

  r = confstr(_CS_POSIX_V6_LP64_OFF64_LDFLAGS, buf, sizeof(buf));
  T_eq_sz(r, len1);

  r = confstr(_CS_POSIX_V7_ILP32_OFF32_CFLAGS, buf, sizeof(buf));
  T_eq_sz(r, len1);

  r = confstr(_CS_POSIX_V7_ILP32_OFFBIG_CFLAGS, buf, sizeof(buf));
  T_eq_sz(r, len1);

  r = confstr(_CS_POSIX_V7_LP64_OFF64_LIBS, buf, sizeof(buf));
  T_eq_sz(r, len1);

  r = confstr(_CS_POSIX_V6_LP64_OFF64_CFLAGS, buf, sizeof(buf));
  T_eq_sz(r, len1);

  errno = 0;
  r = confstr(_CS_PATH, buf, sizeof(buf));
  T_eq_sz(r, 0);
  T_eq_int(errno, EINVAL);
}

static rtems_task Init(rtems_task_argument ignored)
{
  rtems_test_run(ignored, TEST_STATE);
}
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
