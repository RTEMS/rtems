/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup
 *
 * @brief This source file contains the kernel command.
 */

/*
 * Copyright (c) 2022 Chris Johns.  All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/version.h>

static void kernel_summary(void) {
  printf(
    "RTEMS: %d.%d.%d",
    rtems_version_major(), rtems_version_minor(), rtems_version_revision());
  if (rtems_version_control_key_is_valid(rtems_version_control_key())) {
    printf(" (%s)", rtems_version_control_key());
  }
#if RTEMS_SMP
    printf(" SMP:%d cores", rtems_scheduler_get_processor_maximum());
#endif
    printf("\n");
}

static void cpu_summary(void) {
  printf("CPU: " CPU_NAME " (" CPU_MODEL_NAME ")\n");
}

static void bsp_summary(void) {
  printf("BSP: %s\n", rtems_board_support_package());
}

static void tools_summary(void) {
  printf( "Tools: " __VERSION__ "\n");
}

static void opts_summary(void) {
  printf("Options:"
#if RTEMS_DEBUG
         " DEBUG"
#endif
#if RTEMS_MULTIPROCESSING
         " MULTIPROCESSING"
#endif
#if RTEMS_NETWORKING
         " NETWORKING"
#endif
#if RTEMS_PARAVIRT
         " PARAVIRT"
#endif
#if RTEMS_POSIX_API
         " POSIX"
#endif
#if RTEMS_PROFILING
         " PROFILING"
#endif
#if RTEMS_SMP
         " SMP"
#endif
         "\n");
}

static void help(void) {
  printf( "Usage:: rtems <command>\n");
  printf( " where <command> is:\n");
  printf( "  help   : this help\n");
  printf( "  ver    : kernel version\n");
  printf( "  cpu    : kernel version\n");
  printf( "  bsp    : BSP name\n");
  printf( "  tools  : tools version\n");
  printf( "  opts   : options\n");
  printf( "  all    : all commands\n");
}

static int rtems_shell_main_rtems(
  int argc, char *argv[]) {

  if (argc == 1) {
    kernel_summary();
  } else if (argc == 2) {
    if (strcmp(argv[1], "help") == 0) {
      help();
    } else if (strcmp(argv[1], "ver") == 0) {
      kernel_summary();
    } else if (strcmp(argv[1], "cpu") == 0) {
      cpu_summary();
    } else if (strcmp(argv[1], "bsp") == 0) {
      bsp_summary();
    } else if (strcmp(argv[1], "tools") == 0) {
      tools_summary();
    } else if (strcmp(argv[1], "opts") == 0) {
      opts_summary();
    } else if (strcmp(argv[1], "all") == 0) {
      kernel_summary();
      cpu_summary();
      bsp_summary();
      tools_summary();
      opts_summary();
    } else {
      printf("error: invalid command\n");
      return 1;
    }
  } else {
    printf("error: invalid command\n");
    return 1;
  }
  return 0;
}

#define HELP_LINE \
  "rtems <command> (eg. help)"

rtems_shell_cmd_t rtems_shell_RTEMS_Command = {
  "rtems",                             /* name */
  HELP_LINE,                           /* usage */
  "rtems",                             /* topic */
  rtems_shell_main_rtems,              /* command */
  NULL,                                /* alias */
  NULL,                                /* next */
  0500,                                /* mode */
  0,                                   /* uid */
  0                                    /* gid */
};
