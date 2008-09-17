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

#include <rtems/seterr.h>
#include "devfs.h"

int devFS_Show(void)
{
  int                               i;
  rtems_filesystem_location_info_t *temp_loc;
  rtems_device_name_t              *device_name_table;

  temp_loc = &rtems_filesystem_root;
  device_name_table = (rtems_device_name_t *)temp_loc->node_access;
  if (!device_name_table)
    rtems_set_errno_and_return_minus_one( EFAULT );

  for (i = 0; i < rtems_device_table_size; i++){
    if (device_name_table[i].device_name){
        printk("/%s %d %d\n", device_name_table[i].device_name, 
            device_name_table[i].major, device_name_table[i].minor);
    }
  }
  return 0;
}


