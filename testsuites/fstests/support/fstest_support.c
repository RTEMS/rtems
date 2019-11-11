/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
