/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief CPUUSE Command Implementation
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include <rtems.h>
#include <rtems/cpuuse.h>
#include <rtems/printer.h>
#include <rtems/shell.h>
#include "internal.h"

static int rtems_shell_main_cpuuse(
  int   argc,
  char *argv[]
)
{
  /*
   *  When invoked with no arguments, print the report.
   */
  if ( argc == 1 ) {
    rtems_printer printer;
    rtems_print_printer_fprintf(&printer, stdout);
    rtems_cpu_usage_report_with_plugin(&printer);
    return 0;
  }

  /*
   *  When invoked with the single argument -r, reset the statistics.
   */
  if ( argc == 2 && !strcmp( argv[1], "-r" ) ) {
    printf( "Resetting CPU Usage information\n" );
    rtems_cpu_usage_reset();
    return 0;
  }

  /*
   *  OK.  The user did something wrong.
   */
  fprintf( stderr, "%s: [-r]\n", argv[0] );
  return -1;
}

rtems_shell_cmd_t rtems_shell_CPUUSE_Command = {
  .name = "cpuuse",
  .usage = "[-r] print or reset per thread cpu usage",
  .topic = "rtems",
  .command = rtems_shell_main_cpuuse,
  .alias = NULL,
  .next = NULL
};
