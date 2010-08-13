/**
 * @file
 *
 * @ingroup RTEMSMedia
 *
 * @brief Media implementation.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <rtems.h>

#include <rtems/media.h>

rtems_status_code rtems_media_get_device_identifier(
  const char *device_path,
  dev_t *device_identifier
)
{
  int rv = 0;
  struct stat st;

  rv = stat(device_path, &st);
  if (rv != 0) {
    return RTEMS_INVALID_ID;
  }

  *device_identifier = st.st_rdev;

  return RTEMS_SUCCESSFUL;
}
