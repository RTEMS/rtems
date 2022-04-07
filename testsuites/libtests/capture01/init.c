/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
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

#define CONFIGURE_INIT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/capture-cli.h>
#include <rtems/captureimpl.h>
#include <rtems/monitor.h>
#include <rtems/shell.h>
#include <assert.h>
#include <tmacros.h>

#define ASSERT_SC(sc) assert((sc) == RTEMS_SUCCESSFUL)

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

const char rtems_test_name[] = "CAPTURE 1";

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code   sc;
  rtems_task_priority old_priority;
  rtems_mode          old_mode;
  rtems_name          to_name = rtems_build_name('I', 'D', 'L', 'E');;

  rtems_print_printer_fprintf_putc(&rtems_test_printer);

  TEST_BEGIN();

  rtems_task_set_priority(RTEMS_SELF, 20, &old_priority);
  rtems_task_mode(RTEMS_PREEMPT,  RTEMS_PREEMPT_MASK, &old_mode);

  sc = rtems_capture_open (5000, NULL);
  ASSERT_SC(sc);

  sc = rtems_capture_watch_ceiling (100);
  ASSERT_SC(sc);

  sc = rtems_capture_watch_floor (102);
  ASSERT_SC(sc);

  sc = rtems_capture_watch_global (true);
  ASSERT_SC(sc);

  sc = rtems_capture_set_trigger (
    0,
    0,
    to_name,
    0,
    rtems_capture_from_any,
    rtems_capture_switch
  );
  ASSERT_SC(sc);

  rtems_capture_print_watch_list();

  sc = rtems_capture_set_control (true);
  ASSERT_SC(sc);

  capture_test_1();

  sc = rtems_capture_set_control (false);
  ASSERT_SC(sc);

  rtems_capture_print_trace_records ( 22, false );
  rtems_capture_print_trace_records ( 22, false );

  TEST_END();
  exit( 0 );
}
