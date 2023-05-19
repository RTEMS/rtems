/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief RTEMS File System Eval Generic Path
 *  @ingroup LibIOInternal
 */

/*
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
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
