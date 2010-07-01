/**
 * @file
 *
 * @ingroup LibIO
 *
 * @brief mount_and_make_target_path() implementation.
 */

/*
 * Copyright (c) 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>

#include <rtems/libio.h>

int mount_and_make_target_path(
  const char *source,
  const char *target,
  const char *filesystemtype,
  rtems_filesystem_options_t options,
  const void *data
)
{
  int rv = -1;

  if (target != NULL) {
    rv = rtems_mkdir(target, S_IRWXU | S_IRWXG | S_IRWXO);
    if (rv == 0) {
      rv = mount(
        source,
        target,
        filesystemtype,
        options,
        data
      );
    }
  } else {
    errno = EINVAL;
  }

  return rv;
}
