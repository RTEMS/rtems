/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "devfs.h"

int devFS_stat(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  int rv = 0;
  const devFS_node *the_dev = loc->node_access;

  if (the_dev != NULL) {
    buf->st_rdev = rtems_filesystem_make_dev_t( the_dev->major, the_dev->minor );
    buf->st_mode = the_dev->mode;
  } else {
    rv = rtems_filesystem_default_fstat(loc, buf);
  }

  return rv;
}
