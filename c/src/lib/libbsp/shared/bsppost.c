/*
 *  This is a shared BSP post driver hook designed to open
 *  /dev/console for stdin, stdout, and stderr if it exists.
 *  Newlib will automatically associate the file descriptors
 *  with the first thress files opened.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <fcntl.h>

void bsp_postdriver_hook(void)
{
  int stdin_fd, stdout_fd, stderr_fd;
  int error_code = 'S' << 24 | 'T' << 16 | 'D' << 8;

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
    rtems_fatal_error_occurred( error_code | '1' );

  if ((stderr_fd = open("/dev/console", O_WRONLY, 0)) == -1)
    rtems_fatal_error_occurred( error_code | '2' );
}
