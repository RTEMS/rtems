/**
 * @file
 * TERMIOS POSIX behavior on INTR and QUIT characters
 */

/*
 *  COPYRIGHT (c) 1989-2012,2019.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  SPDX-License-Identifier: BSD-2-Clause
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>

#include <signal.h>

rtems_termios_isig_status_code rtems_termios_posix_isig_handler(
  unsigned char             c,
  struct rtems_termios_tty *tty
)
{
  if (c == tty->termios.c_cc[VINTR]) {
    raise(SIGINT);
    return RTEMS_TERMIOS_ISIG_INTERRUPT_READ;
  }

  if (c == tty->termios.c_cc[VQUIT]) {
    raise(SIGQUIT);
    return RTEMS_TERMIOS_ISIG_INTERRUPT_READ;
  }

  return RTEMS_TERMIOS_ISIG_WAS_NOT_PROCESSED;
}
