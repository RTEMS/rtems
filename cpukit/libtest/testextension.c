/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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
#include <rtems/profiling.h>
#include <rtems/bspIo.h>

#if defined(RTEMS_PROFILING)
static bool report_done;

RTEMS_INTERRUPT_LOCK_DEFINE( static, report_lock, "test report" )
#endif

void rtems_test_fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
#if defined(RTEMS_PROFILING)
  rtems_interrupt_lock_context lock_context;
  rtems_printer printer;

  rtems_print_printer_printk( &printer );

  /*
   * Ensures to report only once on SMP machines and ensures that the report is
   * output completely.
   */
  rtems_interrupt_lock_acquire( &report_lock, &lock_context );

  if ( !report_done ) {
    report_done = true;

    printk(
      "\n*** PROFILING REPORT BEGIN %s ***\n",
      rtems_test_name
    );

    rtems_profiling_report_xml(
      rtems_test_name,
      &printer,
      1,
      "  "
    );

    printk(
      "*** PROFILING REPORT END %s ***\n",
      rtems_test_name
    );
  }

  rtems_interrupt_lock_release( &report_lock, &lock_context );
#endif

  (void) source;
  (void) always_set_to_false;
  (void) code;
}
