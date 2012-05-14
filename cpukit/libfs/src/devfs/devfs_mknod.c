/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "devfs.h"

int devFS_mknod(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  mode_t mode,
  dev_t dev
)
{
  int rv = 0;

  if (namelen != 3 || name [0] != 'd' || name [1] != 'e' || name [2] != 'v') {
    if (S_ISBLK(mode) || S_ISCHR(mode)) {
      char *dupname = malloc(namelen);

      if (dupname != NULL) {
        devFS_node *node = parentloc->node_access;

        node->name = dupname;
        node->namelen = namelen;
        node->major = rtems_filesystem_dev_major_t(dev);
        node->minor = rtems_filesystem_dev_minor_t(dev);
        node->mode = mode;
        memcpy(dupname, name, namelen);
      } else {
        errno = ENOMEM;
        rv = -1;
      }
    } else {
      errno = ENOTSUP;
      rv = -1;
    }
  } else {
    if (!S_ISDIR(mode)) {
      errno = ENOTSUP;
      rv = -1;
    }
  }

  return rv;
}
