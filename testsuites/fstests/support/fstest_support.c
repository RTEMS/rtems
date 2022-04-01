/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
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
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <rtems.h>
#include <rtems/userenv.h>

#include "fstest_support.h"
#include "fs_config.h"

#include "fstest.h"
#include <tmacros.h>

void *volatile prevent_compiler_optimizations;

/* Break out of a chroot() environment in C */
static void break_out_of_chroot(void)
{
  int rv;
  struct stat st;

  rtems_libio_use_global_env();

  /* Perform deferred global location releases */
  rv = stat(".", &st);
  rtems_test_assert(rv == 0);

  /* Perform deferred memory frees */
  prevent_compiler_optimizations = malloc(1);
  free(prevent_compiler_optimizations);
}

/*
 *  Main entry point of every filesystem test
 */

rtems_task Init(
    rtems_task_argument ignored)
{
  int rc=0;

  TEST_BEGIN();

  puts( "Initializing filesystem " FILESYSTEM );
  test_initialize_filesystem();

  rc=chroot(BASE_FOR_TEST);
  rtems_test_assert(rc==0);

  test();

  break_out_of_chroot();

  puts( "\n\nShutting down filesystem " FILESYSTEM );
  test_shutdown_filesystem();

  TEST_END();
  rtems_test_exit(0);
}
