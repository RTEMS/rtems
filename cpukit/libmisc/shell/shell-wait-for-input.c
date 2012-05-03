/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/shell.h>

#include <termios.h>
#include <unistd.h>

static rtems_status_code change_serial_settings(int fd, struct termios *term)
{
  rtems_status_code sc = RTEMS_UNSATISFIED;
  int rv = tcgetattr(fd, term);

  if (rv == 0) {
    struct termios new_term = *term;

    new_term.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    new_term.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    new_term.c_cflag &= ~(CSIZE | PARENB);
    new_term.c_cflag |= CS8;

    new_term.c_cc [VMIN] = 0;
    new_term.c_cc [VTIME] = 10;

    rv = tcsetattr(fd, TCSANOW, &new_term);
    if (rv == 0) {
      sc = RTEMS_SUCCESSFUL;
    }
  }

  return sc;
}

static rtems_status_code restore_serial_settings(int fd, struct termios *term)
{
  int rv = tcsetattr(fd, TCSANOW, term);

  return rv == 0 ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED;
}

rtems_status_code rtems_shell_wait_for_input(
  int fd,
  int timeout_in_seconds,
  rtems_shell_wait_for_input_notification notification,
  void *notification_arg
)
{
  struct termios term;
  rtems_status_code sc = change_serial_settings(fd, &term);

  if (sc == RTEMS_SUCCESSFUL) {
    bool input_detected = false;
    int i = 0;

    for (i = 0; i < timeout_in_seconds && !input_detected; ++i) {
      char c;

      (*notification)(fd, timeout_in_seconds - i, notification_arg);

      input_detected = read(fd, &c, sizeof(c)) > 0;
    }

    sc = restore_serial_settings(fd, &term);
    if (sc == RTEMS_SUCCESSFUL) {
      sc = input_detected ? RTEMS_SUCCESSFUL : RTEMS_TIMEOUT;
    }
  }

  return sc;
}
