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
#include <rtems/console.h>
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
  if ( open( CONSOLE_DEVICE_NAME, O_RDONLY, 0 ) != STDIN_FILENO ) {
    /*
     * There may not be a console driver so this is OK.
     */
    return;
  }

  /*
   *  But if we find /dev/console once, we better find it twice more
   *  or something is REALLY wrong.
   */
  if ( open( CONSOLE_DEVICE_NAME, O_WRONLY, 0 ) != STDOUT_FILENO ) {
    _Internal_error( INTERNAL_ERROR_LIBIO_STDOUT_FD_OPEN_FAILED );
  }

  if ( open( CONSOLE_DEVICE_NAME, O_WRONLY, 0 ) != STDERR_FILENO ) {
    _Internal_error( INTERNAL_ERROR_LIBIO_STDERR_FD_OPEN_FAILED );
  }

  atexit(rtems_libio_exit);
}

