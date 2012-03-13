/**
 * @file
 *
 * @ingroup LibIO
 *
 * @brief rtems_filesystem_default_node_type() implementation.
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

#include <rtems/libio.h>

rtems_filesystem_node_types_t rtems_filesystem_default_node_type(
  const rtems_filesystem_location_info_t *loc
)
{
  return RTEMS_FILESYSTEM_INVALID_NODE_TYPE;
}
