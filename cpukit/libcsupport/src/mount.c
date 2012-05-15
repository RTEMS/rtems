/*
 *  mount()
 *
 *  XXX
 *
 *  XXX make sure no required ops are NULL
 *  XXX make sure no optional ops you are using are NULL
 *  XXX unmount should be required.
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <rtems/libio_.h>

RTEMS_CHAIN_DEFINE_EMPTY(rtems_filesystem_mount_table);

/*
 * Default pathconfs.
 */
const rtems_filesystem_limits_and_options_t rtems_filesystem_default_pathconf = {
   5,    /* link_max: count */
   128,  /* max_canon: max formatted input line size */
   7,    /* max_input: max input line size */
   255,  /* name_max: max name */
   255,  /* path_max: max path */
   1024, /* pipe_buf: pipe buffer size */
   0,    /* posix_async_io: async IO supported on fs, 0=no, 1=yes */
   0 ,   /* posix_chown_restrictions: can chown: 0=no, 1=yes */
   1,    /* posix_no_trunc: error on filenames > max name, 0=no, 1=yes */
   0,    /* posix_prio_io: priority IO, 0=no, 1=yes */
   0,    /* posix_sync_io: file can be sync'ed, 0=no, 1=yes */
   0     /* posix_vdisable: special char processing, 0=no, 1=yes */
};

static rtems_filesystem_mount_table_entry_t *alloc_mount_table_entry(
  const char *source_or_null,
  const char *target_or_null,
  const char *filesystemtype,
  size_t *target_length_ptr
)
{
  const char *target = target_or_null != NULL ? target_or_null : "/";
  size_t filesystemtype_size = strlen( filesystemtype ) + 1;
  size_t source_size = source_or_null != NULL ?
    strlen( source_or_null ) + 1 : 0;
  size_t target_size = strlen( target ) + 1;
  size_t size = sizeof( rtems_filesystem_mount_table_entry_t )
    + filesystemtype_size + source_size + target_size
    + sizeof( rtems_filesystem_global_location_t );
  rtems_filesystem_mount_table_entry_t *mt_entry = calloc( 1, size );

  if ( mt_entry != NULL ) {
    rtems_filesystem_global_location_t *mt_fs_root =
      (rtems_filesystem_global_location_t *)
        ((char *) mt_entry + sizeof( *mt_entry ));
    char *str = (char *) mt_fs_root + sizeof( *mt_fs_root );

    memcpy( str, filesystemtype, filesystemtype_size );
    mt_entry->type = str;
    str += filesystemtype_size;

    memcpy( str, source_or_null, source_size );
    mt_entry->dev = str;
    str += source_size;

    memcpy( str, target, target_size );
    mt_entry->target = str;
    str += target_size;

    mt_entry->mounted = true;
    mt_entry->mt_fs_root = mt_fs_root;
    mt_entry->pathconf_limits_and_options = &rtems_filesystem_default_pathconf;

    mt_fs_root->location.mt_entry = mt_entry;
    mt_fs_root->reference_count = 1;

    rtems_chain_initialize(
      &mt_entry->location_chain,
      mt_fs_root,
      1,
      sizeof(*mt_fs_root)
    );
  } else {
    free( mt_entry );
  }

  *target_length_ptr = target_size - 1;

  return mt_entry;
}

static int register_subordinate_file_system(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const char *target
)
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_PERMS_RWX
    | RTEMS_FS_FOLLOW_LINK;
  rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, target, eval_flags );

  if ( !rtems_filesystem_location_is_root( currentloc ) ) {
    rtems_filesystem_location_info_t targetloc;
    rtems_filesystem_global_location_t *mt_point_node;

    rtems_filesystem_eval_path_extract_currentloc( &ctx, &targetloc );
    mt_point_node = rtems_filesystem_location_transform_to_global( &targetloc );
    mt_entry->mt_point_node = mt_point_node;
    rv = (*mt_point_node->location.mt_entry->ops->mount_h)( mt_entry );
    if ( rv == 0 ) {
      rtems_filesystem_mt_lock();
      rtems_chain_append_unprotected(
        &rtems_filesystem_mount_table,
        &mt_entry->mt_node
      );
      rtems_filesystem_mt_unlock();
    } else {
      rtems_filesystem_global_location_release( mt_point_node );
    }
  } else {
    rtems_filesystem_eval_path_error( &ctx, EBUSY );
    rv = -1;
  }

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}

static int register_root_file_system(
  rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  int rv = 0;

  rtems_filesystem_mt_lock();
  if ( rtems_chain_is_empty( &rtems_filesystem_mount_table ) ) {
    rtems_chain_append_unprotected(
      &rtems_filesystem_mount_table,
      &mt_entry->mt_node
    );
  } else {
    errno = EINVAL;
    rv = -1;
  }
  rtems_filesystem_mt_unlock();

  if ( rv == 0 ) {
    rtems_filesystem_global_location_t *new_fs_root =
      rtems_filesystem_global_location_obtain( &mt_entry->mt_fs_root );
    rtems_filesystem_global_location_t *new_fs_current =
      rtems_filesystem_global_location_obtain( &mt_entry->mt_fs_root );

    rtems_filesystem_global_location_assign(
      &rtems_filesystem_root,
      new_fs_root
    );
    rtems_filesystem_global_location_assign(
      &rtems_filesystem_current,
      new_fs_current
    );
  }

  return rv;
}

int mount(
  const char                 *source,
  const char                 *target,
  const char                 *filesystemtype,
  rtems_filesystem_options_t options,
  const void                 *data
)
{
  int rv = 0;

  if (
    options == RTEMS_FILESYSTEM_READ_ONLY
      || options == RTEMS_FILESYSTEM_READ_WRITE
  ) {
    rtems_filesystem_fsmount_me_t fsmount_me_h =
      rtems_filesystem_get_mount_handler( filesystemtype );

    if ( fsmount_me_h != NULL ) {
      size_t target_length = 0;
      rtems_filesystem_mount_table_entry_t *mt_entry = alloc_mount_table_entry(
        source,
        target,
        filesystemtype,
        &target_length
      );

      if ( mt_entry != NULL ) {
        mt_entry->writeable = options == RTEMS_FILESYSTEM_READ_WRITE;

        rv = (*fsmount_me_h)( mt_entry, data );
        if ( rv == 0 ) {
          if ( target != NULL ) {
            rv = register_subordinate_file_system( mt_entry, target );
          } else {
            rv = register_root_file_system( mt_entry );
          }

          if ( rv != 0 ) {
            (*mt_entry->ops->fsunmount_me_h)( mt_entry );
          }
        }

        if ( rv != 0 ) {
          free( mt_entry );
        }
      } else {
        errno = ENOMEM;
        rv = -1;
      }
    } else {
      errno = EINVAL;
      rv = -1;
    }
  } else {
    errno = EINVAL;
    rv = -1;
  }

  return rv;
}
