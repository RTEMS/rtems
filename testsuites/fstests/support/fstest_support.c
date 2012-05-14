/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> /* exit */
#include <sys/stat.h>
#include <sys/types.h>

#include "rtems.h"

#include "fstest_support.h"
#include "fs_config.h"

#include "fstest.h"
#include "pmacros.h"

/* Break out of a chroot() environment in C */
static void break_out_of_chroot(void)
{
  chroot("/");
}

/*
 *  Main entry point of every filesystem test
 */

rtems_task Init(
    rtems_task_argument ignored)
{
  int rc=0;
  puts( "\n\n*** FILE SYSTEM TEST ( " FILESYSTEM " ) ***" );

  puts( "Initializing filesystem " FILESYSTEM );
  test_initialize_filesystem();

  rc=chroot(BASE_FOR_TEST);
  rtems_test_assert(rc==0);

  test();

  break_out_of_chroot();

  puts( "\n\nShutting down filesystem " FILESYSTEM );
  test_shutdown_filesystem();

  puts( "*** END OF FILE SYSTEM TEST ( " FILESYSTEM " ) ***" );
  rtems_test_exit(0);
}
