/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "devfs.h"

void devFS_Show(void)
{
  rtems_filesystem_location_info_t *rootloc = &rtems_filesystem_root->location;

  if (rootloc->mt_entry->ops == &devFS_ops) {
    const devFS_data *data = devFS_get_data(rootloc);
    size_t i = 0;
    size_t n = data->count;
    devFS_node *nodes = data->nodes;

    for (i = 0; i < n; ++i) {
      devFS_node *current = nodes + i;

      if (current->name != NULL) {
        size_t j = 0;
        size_t m = current->namelen;

        printk("/");
        for (j = 0; j < m; ++j) {
          printk("%c", current->name [j]);
        }
        printk(
          " %lu %lu\n",
          (unsigned long) current->major,
          (unsigned long) current->minor
        );
      }
    }
  }
}
