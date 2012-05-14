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

rtems_filesystem_node_types_t rtems_filesystem_node_type(
  const rtems_filesystem_location_info_t *loc
)
{
  rtems_filesystem_node_types_t type;

  rtems_filesystem_instance_lock(loc);
  type = (*loc->mt_entry->ops->node_type_h)(loc);
  rtems_filesystem_instance_unlock(loc);

  return type;
}
