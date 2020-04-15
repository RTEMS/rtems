/**
 * @file
 *
 * @brief Default Kernel Event Filter Handler
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

int rtems_filesystem_default_kqfilter(
  rtems_libio_t *iop,
  struct knote  *kn
)
{
  return EINVAL;
}

int rtems_termios_kqfilter(
  rtems_libio_t *iop,
  struct knote  *kn
) RTEMS_WEAK_ALIAS( rtems_filesystem_default_kqfilter );
