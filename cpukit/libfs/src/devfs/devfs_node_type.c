/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "devfs.h"

rtems_filesystem_node_types_t devFS_node_type(
  const rtems_filesystem_location_info_t *loc
)
{
  /*
   * There is only one type of node: device
   */

  return RTEMS_FILESYSTEM_DEVICE;
}


