/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <string.h>

#include "devfs.h"

static devFS_node *devFS_search_node(
  const devFS_data *data,
  const char *path,
  size_t pathlen,
  devFS_node **free_node_ptr
)
{
  size_t i = 0;
  size_t n = data->count;
  devFS_node *nodes = data->nodes;
  devFS_node *node = NULL;
  devFS_node *free_node = NULL;

  for (i = 0; (free_node == NULL || node == NULL) && i < n; ++i) {
    devFS_node *current = nodes + i;

    if (current->name != NULL) {
      if (
        current->namelen == pathlen
          && memcmp(current->name, path, pathlen) == 0
      ) {
        node = current;
      }
    } else {
      free_node = current;
    }
  }

  *free_node_ptr = free_node;

  return node;
}

void devFS_eval_path(
  rtems_filesystem_eval_path_context_t *ctx
)
{
  rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_get_currentloc(ctx);
  devFS_node *free_node;
  devFS_node *node = devFS_search_node(
    devFS_get_data(currentloc),
    rtems_filesystem_eval_path_get_path(ctx),
    rtems_filesystem_eval_path_get_pathlen(ctx),
    &free_node
  );
  int eval_flags = rtems_filesystem_eval_path_get_flags(ctx);

  if (node != NULL) {
    if ((eval_flags & RTEMS_FS_EXCLUSIVE) == 0) {
      currentloc->node_access = node;
      rtems_filesystem_eval_path_clear_path(ctx);
    } else {
      rtems_filesystem_eval_path_error(ctx, EEXIST);
    }
  } else {
    if ((eval_flags & RTEMS_FS_MAKE) != 0) {
      if (free_node != NULL) {
        free_node->mode = S_IRWXU | S_IRWXG | S_IRWXO;
        currentloc->node_access = free_node;
        rtems_filesystem_eval_path_set_token(
          ctx,
          rtems_filesystem_eval_path_get_path(ctx),
          rtems_filesystem_eval_path_get_pathlen(ctx)
        );
        rtems_filesystem_eval_path_clear_path(ctx);
      } else {
        rtems_filesystem_eval_path_error(ctx, ENOSPC);
      }
    } else {
      rtems_filesystem_eval_path_error(ctx, ENOENT);
    }
  }
}
