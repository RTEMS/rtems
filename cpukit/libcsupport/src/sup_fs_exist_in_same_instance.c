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
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/libio_.h>

int rtems_filesystem_location_exists_in_same_fs_instance_as(
  const rtems_filesystem_location_info_t *a,
  const rtems_filesystem_location_info_t *b
)
{
  int rv = -1;

  if (
    !rtems_filesystem_location_is_null( a )
      && !rtems_filesystem_location_is_null( b )
  ) {
    if ( a->mt_entry == b->mt_entry ) {
      rv = 0;
    } else {
      errno = EXDEV;
    }
  }

  return rv;
}
