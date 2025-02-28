/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief Shell Wait for Input Implementation
 */

/*
 * Copyright (C) 2011 embedded brains GmbH & Co. KG
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

rtems_status_code rtems_shell_wait_for_explicit_input(
  int fd,
  int timeout_in_seconds,
  rtems_shell_wait_for_input_notification notification,
  void *notification_arg,
  int desired_input
)
{
  struct termios term;
  rtems_status_code sc = change_serial_settings(fd, &term);

  if (sc == RTEMS_SUCCESSFUL) {
    bool input_detected = false;
    int i = 0;

    for (i = 0; i < timeout_in_seconds && !input_detected; ++i) {
      unsigned char c;

      (*notification)(fd, timeout_in_seconds - i, notification_arg);

      input_detected = read(fd, &c, sizeof(c)) > 0
        && (desired_input == -1 || desired_input == c);
    }

    sc = restore_serial_settings(fd, &term);
    if (sc == RTEMS_SUCCESSFUL) {
      sc = input_detected ? RTEMS_SUCCESSFUL : RTEMS_TIMEOUT;
    }
  }

  return sc;
}

rtems_status_code rtems_shell_wait_for_input(
  int fd,
  int timeout_in_seconds,
  rtems_shell_wait_for_input_notification notification,
  void *notification_arg
)
{
  return rtems_shell_wait_for_explicit_input(
    fd,
    timeout_in_seconds,
    notification,
    notification_arg,
    -1
  );
}
