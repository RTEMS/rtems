/*
 *  Evaluation IMFS Node Support Routines
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

static bool IMFS_eval_is_directory(
  rtems_filesystem_eval_path_context_t *ctx,
  void *arg
)
{
  rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_get_currentloc( ctx );
  IMFS_jnode_t *node = currentloc->node_access;

  return IMFS_is_directory( node );
}

static IMFS_jnode_t *IMFS_search_in_directory(
  IMFS_jnode_t *dir,
  const char *token,
  size_t tokenlen
)
{
  if ( rtems_filesystem_is_current_directory( token, tokenlen ) ) {
    return dir;
  } else {
    if ( rtems_filesystem_is_parent_directory( token, tokenlen ) ) {
      return dir->Parent;
    } else {
      rtems_chain_control *entries = &dir->info.directory.Entries;
      rtems_chain_node *current = rtems_chain_first( entries );
      rtems_chain_node *tail = rtems_chain_tail( entries );

      while ( current != tail ) {
        IMFS_jnode_t *entry = (IMFS_jnode_t *) current;
        bool match = strncmp( entry->name, token, tokenlen ) == 0
          && entry->name [tokenlen] == '\0';

        if ( match ) {
          return entry;
        }

        current = rtems_chain_next( current );
      }

      return NULL;
    }
  }
}

static rtems_filesystem_global_location_t **IMFS_is_mount_point(
  IMFS_jnode_t *node,
  IMFS_jnode_types_t type
)
{
  rtems_filesystem_global_location_t **fs_root_ptr = NULL;

  if ( type == IMFS_DIRECTORY ) {
    if ( node->info.directory.mt_fs != NULL ) {
      fs_root_ptr = &node->info.directory.mt_fs->mt_fs_root;
    }
  }

  return fs_root_ptr;
}

static rtems_filesystem_eval_path_generic_status IMFS_eval_token(
  rtems_filesystem_eval_path_context_t *ctx,
  void *arg,
  const char *token,
  size_t tokenlen
)
{
  rtems_filesystem_eval_path_generic_status status =
    RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
  rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_get_currentloc( ctx );
  IMFS_jnode_t *dir = currentloc->node_access;
  bool access_ok = rtems_filesystem_eval_path_check_access(
    ctx,
    RTEMS_FS_PERMS_EXEC,
    dir->st_mode,
    dir->st_uid,
    dir->st_gid
  );

  if ( access_ok ) {
    IMFS_jnode_t *entry = IMFS_search_in_directory( dir, token, tokenlen );

    if ( entry != NULL ) {
      bool terminal = !rtems_filesystem_eval_path_has_path( ctx );
      int eval_flags = rtems_filesystem_eval_path_get_flags( ctx );
      bool follow_hard_link = (eval_flags & RTEMS_FS_FOLLOW_HARD_LINK) != 0;
      bool follow_sym_link = (eval_flags & RTEMS_FS_FOLLOW_SYM_LINK) != 0;
      IMFS_jnode_types_t type = IMFS_type( entry );

      rtems_filesystem_eval_path_clear_token( ctx );

      if ( type == IMFS_HARD_LINK && (follow_hard_link || !terminal)) {
        entry = entry->info.hard_link.link_node;
      }

      if ( type == IMFS_SYM_LINK && (follow_sym_link || !terminal)) {
        const char *target = entry->info.sym_link.name;

        rtems_filesystem_eval_path_recursive( ctx, target, strlen( target ) );
      } else {
        rtems_filesystem_global_location_t **fs_root_ptr =
          IMFS_is_mount_point( entry, type );

        if ( fs_root_ptr == NULL ) {
          --dir->reference_count;
          ++entry->reference_count;
          currentloc->node_access = entry;
          IMFS_Set_handlers( currentloc );

          if ( !terminal ) {
            status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
          }
        } else {
          access_ok = rtems_filesystem_eval_path_check_access(
            ctx,
            RTEMS_FS_PERMS_EXEC,
            entry->st_mode,
            entry->st_uid,
            entry->st_gid
          );
          if ( access_ok ) {
            rtems_filesystem_eval_path_restart( ctx, fs_root_ptr );
          }
        }
      }
    } else {
      status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_NO_ENTRY;
    }
  }

  return status;
}

static const rtems_filesystem_eval_path_generic_config IMFS_eval_config = {
  .is_directory = IMFS_eval_is_directory,
  .eval_token = IMFS_eval_token
};

void IMFS_eval_path( rtems_filesystem_eval_path_context_t *ctx )
{
  rtems_filesystem_eval_path_generic( ctx, NULL, &IMFS_eval_config );
}
