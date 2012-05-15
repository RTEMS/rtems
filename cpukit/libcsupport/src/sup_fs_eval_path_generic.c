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

static bool is_fs_root( const rtems_filesystem_location_info_t *loc )
{
  const rtems_filesystem_mount_table_entry_t *mt_entry = loc->mt_entry;
  const rtems_filesystem_location_info_t *mt_fs_root =
    &mt_entry->mt_fs_root->location;

  return (*mt_entry->ops->are_nodes_equal_h)( loc, mt_fs_root );
}

static bool is_eval_path_root(
  const rtems_filesystem_eval_path_context_t *ctx,
  const rtems_filesystem_location_info_t *loc
)
{
  const rtems_filesystem_mount_table_entry_t *mt_entry = loc->mt_entry;
  const rtems_filesystem_location_info_t *rootloc = &ctx->rootloc->location;

  return mt_entry == rootloc->mt_entry
    && (*mt_entry->ops->are_nodes_equal_h)( loc, rootloc );
}

void rtems_filesystem_eval_path_generic(
  rtems_filesystem_eval_path_context_t *ctx,
  void *arg,
  const rtems_filesystem_eval_path_generic_config *config
)
{
  rtems_filesystem_eval_path_generic_status status =
    RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;

  while (status == RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE) {
    const char *token;
    size_t tokenlen;

    rtems_filesystem_eval_path_get_next_token(ctx, &token, &tokenlen);

    if (tokenlen > 0) {
      if ((*config->is_directory)(ctx, arg)) {
        if (rtems_filesystem_is_current_directory(token, tokenlen)) {
          if (rtems_filesystem_eval_path_has_path(ctx)) {
            status = (*config->eval_token)(ctx, arg, ".", 1);
          } else {
            int eval_flags = rtems_filesystem_eval_path_get_flags(ctx);

            if ((eval_flags & RTEMS_FS_REJECT_TERMINAL_DOT) == 0) {
              status = (*config->eval_token)(ctx, arg, ".", 1);
            } else {
              rtems_filesystem_eval_path_error(ctx, EINVAL);
              status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
            }
          }
        } else if (rtems_filesystem_is_parent_directory(token, tokenlen)) {
          rtems_filesystem_location_info_t *currentloc =
            rtems_filesystem_eval_path_get_currentloc( ctx );

          if (is_eval_path_root(ctx, currentloc)) {
            /* This prevents the escape from a chroot() environment */
            status = (*config->eval_token)(ctx, arg, ".", 1);
          } else if (is_fs_root(currentloc)) {
            if (currentloc->mt_entry->mt_point_node != NULL) {
              rtems_filesystem_eval_path_put_back_token(ctx);
              rtems_filesystem_eval_path_restart(
                ctx,
                &currentloc->mt_entry->mt_point_node
              );
              status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
            } else {
              /* This is the root file system */
              status = (*config->eval_token)(ctx, arg, ".", 1);
            }
          } else {
            status = (*config->eval_token)(ctx, arg, "..", 2);
          }
        } else {
          status = (*config->eval_token)(ctx, arg, token, tokenlen);
        }

        if (status == RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_NO_ENTRY) {
          if (rtems_filesystem_eval_path_has_path(ctx)) {
            int eval_flags;

            rtems_filesystem_eval_path_eat_delimiter(ctx);
            eval_flags = rtems_filesystem_eval_path_get_flags(ctx);
            if (
              (eval_flags & RTEMS_FS_ACCEPT_RESIDUAL_DELIMITERS) == 0
                || rtems_filesystem_eval_path_has_path(ctx)
            ) {
              rtems_filesystem_eval_path_error(ctx, ENOENT);
            }
          }
        }
      } else {
        rtems_filesystem_eval_path_error(ctx, ENOTDIR);
        status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
      }
    } else {
      status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
    }
  }
}
