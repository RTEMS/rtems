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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <rtems/seterr.h>
#include "devfs.h"

int devFS_mknod(
  const char                        *path,
  mode_t                             mode, 
  dev_t                              dev,  
  rtems_filesystem_location_info_t  *pathloc 
)
{
  int                       i;
  int                       slot;
  rtems_device_name_t      *device_name_table;
  rtems_device_major_number major;
  rtems_device_minor_number minor;
  ISR_Level                 level;

  /*
   * This is a special case. In rtems_filesystem_initialize, 
   * a special device '/dev' will be created. We check this 
   * condition and do not create the '/dev' and the 'path' 
   * actually passed in is 'dev', not '/dev'. Just return 0 to
   * indicate we are OK.
   */

  if ((path[0] == 'd') && (path[1] == 'e') && 
      (path[2] == 'v') && (path[3] == '\0'))
      return 0;

  /* must be a character device or a block device */
  if (!S_ISBLK(mode) && !S_ISCHR(mode))
    rtems_set_errno_and_return_minus_one( EINVAL );
  else
    rtems_filesystem_split_dev_t(dev, major, minor);

  /* Find an empty slot in device name table */
  device_name_table = (rtems_device_name_t *)pathloc->node_access;
  if (!device_name_table)
    rtems_set_errno_and_return_minus_one( EFAULT );

  for (slot = -1, i = 0; i < rtems_device_table_size; i++){
      if (device_name_table[i].device_name == NULL)
          slot = i;
      else
          if (strcmp(path, device_name_table[i].device_name) == 0)
              rtems_set_errno_and_return_minus_one( EEXIST );
  }

  if (slot == -1)
      rtems_set_errno_and_return_minus_one( ENOMEM );

  _ISR_Disable(level);
  device_name_table[slot].device_name  = (char *)path;
  device_name_table[slot].device_name_length = strlen(path);
  device_name_table[slot].major = major;
  device_name_table[slot].minor = minor;
  device_name_table[slot].mode  = mode;
  _ISR_Enable(level);

  return 0;
}

