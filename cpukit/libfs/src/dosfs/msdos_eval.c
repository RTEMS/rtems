/*
 *  MSDOS evaluation routines
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

#include <rtems/libio_.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"

/* msdos_set_handlers --
 *     Set handlers for the node with specified type(i.e. handlers for file
 *     or directory).
 *
 * PARAMETERS:
 *     loc - node description
 *
 * RETURNS:
 *     None
 */
static void
msdos_set_handlers(rtems_filesystem_location_info_t *loc)
{
    msdos_fs_info_t *fs_info = loc->mt_entry->fs_info;
    fat_file_fd_t   *fat_fd = loc->node_access;

    if (fat_fd->fat_file_type == FAT_DIRECTORY)
        loc->handlers = fs_info->directory_handlers;
    else
        loc->handlers = fs_info->file_handlers;
}

static bool msdos_is_directory(
  rtems_filesystem_eval_path_context_t *ctx,
  void *arg
)
{
  rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_get_currentloc( ctx );
  fat_file_fd_t *fat_fd = currentloc->node_access;

  return fat_fd->fat_file_type == MSDOS_DIRECTORY;
}

static rtems_filesystem_eval_path_generic_status msdos_eval_token(
  rtems_filesystem_eval_path_context_t *ctx,
  void *arg,
  const char *token,
  size_t tokenlen
)
{
  rtems_filesystem_eval_path_generic_status status =
    RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;

  if (rtems_filesystem_is_current_directory(token, tokenlen)) {
    rtems_filesystem_eval_path_clear_token(ctx);
    status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
  } else {
    rtems_filesystem_location_info_t *currentloc =
      rtems_filesystem_eval_path_get_currentloc(ctx);
    int rc = msdos_find_name(currentloc, token, tokenlen);

    if (rc == RC_OK) {
      rtems_filesystem_eval_path_clear_token(ctx);
      msdos_set_handlers(currentloc);
      if (rtems_filesystem_eval_path_has_path(ctx)) {
        status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
      }
    } else if (rc == MSDOS_NAME_NOT_FOUND_ERR) {
      status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_NO_ENTRY;
    } else {
      rtems_filesystem_eval_path_error(ctx, 0);
    }
  }

  return status;
}

static const rtems_filesystem_eval_path_generic_config msdos_eval_config = {
  .is_directory = msdos_is_directory,
  .eval_token = msdos_eval_token
};

void msdos_eval_path(rtems_filesystem_eval_path_context_t *ctx)
{
  rtems_filesystem_eval_path_generic(ctx, NULL, &msdos_eval_config);
}
