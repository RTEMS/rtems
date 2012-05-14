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

void rtems_filesystem_location_clone(
  rtems_filesystem_location_info_t *clone,
  const rtems_filesystem_location_info_t *master
)
{
  int rv = 0;

  clone = rtems_filesystem_location_copy( clone, master );
  rv = (*clone->mt_entry->ops->clonenod_h)( clone );
  if ( rv != 0 ) {
    rtems_filesystem_location_remove_from_mt_entry( clone );
    rtems_filesystem_location_initialize_to_null( clone );
  }
}
