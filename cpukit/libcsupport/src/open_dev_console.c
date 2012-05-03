/*
 *  open_dev_console - open /dev/console
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <fcntl.h>

/*
 *  This is a replaceable stub which opens the console, if present.
 */
void open_dev_console(void)
{
  int      stdin_fd;
  int      stdout_fd;
  int      stderr_fd;

  /*
   * Attempt to open /dev/console.
   */
  if ((stdin_fd = open("/dev/console", O_RDONLY, 0)) == -1) {
    /*
     * There may not be a console driver so this is OK.
     */
    return;
  }

  /*
   *  But if we find /dev/console once, we better find it twice more
   *  or something is REALLY wrong.
   */
  if ((stdout_fd = open("/dev/console", O_WRONLY, 0)) == -1)
    rtems_fatal_error_occurred( 0x55544431 );  /* error STD1 */

  if ((stderr_fd = open("/dev/console", O_WRONLY, 0)) == -1)
    rtems_fatal_error_occurred( 0x55544432 );  /* error STD2 */
}

