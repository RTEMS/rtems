/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief stackuse Command Implementation
 */

/*
 *
 * COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
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

#include <rtems.h>
#include <rtems/printer.h>
#include <rtems/stackchk.h>
#include <rtems/shell.h>
#include <rtems/score/threadimpl.h>
#include "internal.h"

static int rtems_shell_main_stackuse(
  int   argc RTEMS_UNUSED,
  char *argv[] RTEMS_UNUSED
)
{
  rtems_printer printer;
  rtems_print_printer_printf(&printer);
  rtems_stack_checker_report_usage_with_plugin( &printer );
  return 0;
}

rtems_shell_cmd_t rtems_shell_STACKUSE_Command = {
  .name = "stackuse",
  .usage = "print per thread stack usage",
  .topic = "rtems",
  .command = rtems_shell_main_stackuse,
  .alias = NULL,
  .next = NULL
};
