/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Debugger test.
 *
 *  Copyright (c) 2016 Chris Johns (chrisj@rtems.org)
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

#define CONFIGURE_INIT
#include "system.h"

#include "tmacros.h"

#include <rtems/rtems-debugger.h>

#include "system.h"

static void test(void)
{
  rtems_test_assert(rtems_debugger_start("test", "something",
                                         3, 10, &rtems_test_printer) < 0);
  rtems_test_assert(rtems_debugger_register_test_remote() == 0);
  rtems_test_assert(rtems_debugger_start("test", "something",
                                         3, 10, &rtems_test_printer) == 0);
  rtems_debugger_set_verbose(true);
  rtems_test_assert(rtems_debugger_remote_debug(true) == 0);

  /*
   * This thread is suspended when the debugger is active until the debugger
   * disconnects.
   */
  sleep(1);
}

const char rtems_test_name[] = "DEBUGGER01";

rtems_task Init(rtems_task_argument argument)
{
  TEST_BEGIN();

  test();

  TEST_END();
}
