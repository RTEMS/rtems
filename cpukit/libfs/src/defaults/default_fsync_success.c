/**
 * @file
 *
 * @brief RTEMS Default Filesystem succeeds synchronizing file's in-core state
 * @ingroup LibIOFSOps File System Operations 
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/libio_.h>

int rtems_filesystem_default_fsync_or_fdatasync_success(
  rtems_libio_t *iop
)
{
  return 0;
}
