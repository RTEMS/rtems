/**
 * @file
 *
 * @brief Default Poll Handler
 *
 * @ingroup LibIOFSHandler
 */

/*
 * Copyright (c) 2013, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/libio_.h>

#include <sys/poll.h>

int rtems_filesystem_default_poll(
  rtems_libio_t *iop,
  int            events
)
{
  return POLLERR;
}

int rtems_termios_poll(
  rtems_libio_t *iop,
  int            events
) RTEMS_WEAK_ALIAS( rtems_filesystem_default_poll );
