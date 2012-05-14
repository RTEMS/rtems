/*
 *  Termios initialization routine
 *
 *  Author:
 *    W. Eric Norum
 *    Saskatchewan Accelerator Laboratory
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@skatter.usask.ca
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct rtems_termios_tty *rtems_termios_ttyHead;
struct rtems_termios_tty *rtems_termios_ttyTail;
rtems_id rtems_termios_ttyMutex;

void
rtems_termios_initialize (void)
{
  rtems_status_code sc;

  /*
   * Create the mutex semaphore for the tty list
   */
  if (!rtems_termios_ttyMutex) {
    sc = rtems_semaphore_create (
      rtems_build_name ('T', 'R', 'm', 'i'),
      1,
      RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
      RTEMS_NO_PRIORITY,
      &rtems_termios_ttyMutex);
    if (sc != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred (sc);
  }
}
