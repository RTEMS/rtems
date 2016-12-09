/*
 *  open_dev_console - open /dev/console
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/*
 *  This is a replaceable stub which opens the console, if present.
 */
void rtems_libio_post_driver(void)
{
  /*
   * Attempt to open /dev/console.
   */
  if (open("/dev/console", O_RDONLY, 0) != STDIN_FILENO) {
    /*
     * There may not be a console driver so this is OK.
     */
    return;
  }

  /*
   *  But if we find /dev/console once, we better find it twice more
   *  or something is REALLY wrong.
   */
  if (open("/dev/console", O_WRONLY, 0) != STDOUT_FILENO) {
    rtems_fatal_error_occurred( 0x55544431 );
  }

  if (open("/dev/console", O_WRONLY, 0) != STDERR_FILENO) {
    rtems_fatal_error_occurred( 0x55544432 );
  }

  atexit(rtems_libio_exit);
}

