/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "devfs.h"

int devFS_node_type(
  rtems_filesystem_location_info_t  *pathloc
)
{
  /*
   * There is only one type of node: device
   */

  return RTEMS_FILESYSTEM_DEVICE;
}


