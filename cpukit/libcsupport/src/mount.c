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
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <rtems/chain.h>
#include <rtems/seterr.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <rtems/libio_.h>

static RTEMS_CHAIN_DEFINE_EMPTY(mount_chain);

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

static bool is_node_fs_root(
  const rtems_filesystem_mount_table_entry_t *mt_entry,
  void *arg
)
{
  return arg == mt_entry->mt_fs_root.node_access;
}

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
  size_t target_length = strlen( target );
  size_t size = sizeof( rtems_filesystem_mount_table_entry_t )
    + filesystemtype_size + source_size + target_length + 1;
  rtems_filesystem_mount_table_entry_t *mt_entry = calloc( 1, size );

  if ( mt_entry != NULL ) {
    char *str = (char *) mt_entry + sizeof( *mt_entry );

    strcpy( str, filesystemtype );
    mt_entry->type = str;
    str += filesystemtype_size;

    if ( source_or_null != NULL ) {
      strcpy( str, source_or_null );
      mt_entry->dev = str;
      str += source_size;
    }

    strcpy( str, target );
    mt_entry->target = str;
  }

  *target_length_ptr = target_length;

  return mt_entry;
}

/*
 *  mount
 *
 *  This routine will attempt to mount a new file system at the specified
 *  mount point. A series of tests will be run to determine if any of the
 *  following reasons exist to prevent the mount operation:
 *
 * 	1) The file system type or options are not valid
 *	2) No new file system root node is specified
 * 	3) The selected file system has already been mounted
 * 	4) The mount point exists with the proper permissions to allow mounting
 *	5) The selected mount point already has a file system mounted to it
 *
 */

int mount(
  const char                 *source,
  const char                 *target,
  const char                 *filesystemtype,
  rtems_filesystem_options_t options,
  const void                 *data
)
{
  rtems_filesystem_fsmount_me_t mount_h = NULL;
  rtems_filesystem_location_info_t      loc;
  rtems_filesystem_mount_table_entry_t *mt_entry = NULL;
  rtems_filesystem_location_info_t     *loc_to_free = NULL;
  bool has_target = target != NULL;
  size_t target_length = 0;

  /*
   *  Are the file system options valid?
   */

  if ( options != RTEMS_FILESYSTEM_READ_ONLY &&
       options != RTEMS_FILESYSTEM_READ_WRITE )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   *  Get mount handler
   */
  mount_h = rtems_filesystem_get_mount_handler( filesystemtype );
  if ( !mount_h )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   * Allocate a mount table entry
   */
  mt_entry = alloc_mount_table_entry(
    source,
    target,
    filesystemtype,
    &target_length
  );
  if ( !mt_entry )
    rtems_set_errno_and_return_minus_one( ENOMEM );

  mt_entry->mt_fs_root.mt_entry = mt_entry;
  mt_entry->options = options;
  mt_entry->pathconf_limits_and_options = rtems_filesystem_default_pathconf;

  /*
   *  The mount_point should be a directory with read/write/execute
   *  permissions in the existing tree.
   */

  if ( has_target ) {
    if ( rtems_filesystem_evaluate_path(
           target, target_length, RTEMS_LIBIO_PERMS_RWX, &loc, true ) == -1 )
      goto cleanup_and_bail;

    loc_to_free = &loc;

    /*
     *  Test to see if it is a directory
     */

    if ( loc.ops->node_type_h( &loc ) != RTEMS_FILESYSTEM_DIRECTORY ) {
      errno = ENOTDIR;
      goto cleanup_and_bail;
    }

    /*
     *  You can only mount one file system onto a single mount point.
     */

    if ( rtems_filesystem_mount_iterate( is_node_fs_root, loc.node_access ) ) {
      errno = EBUSY;
      goto cleanup_and_bail;
    }

    /*
     *  This must be a good mount point, so move the location information
     *  into the allocated mount entry.  Note:  the information that
     *  may have been allocated in loc should not be sent to freenode
     *  until the system is unmounted.  It may be needed to correctly
     *  traverse the tree.
     */

    mt_entry->mt_point_node.node_access = loc.node_access;
    mt_entry->mt_point_node.handlers = loc.handlers;
    mt_entry->mt_point_node.ops = loc.ops;
    mt_entry->mt_point_node.mt_entry = loc.mt_entry;

    /*
     *  This link to the parent is only done when we are dealing with system
     *  below the base file system
     */

    if ( loc.ops->mount_h( mt_entry ) ) {
      goto cleanup_and_bail;
    }
  } else {
    /*
     * Do we already have a base file system ?
     */
    if ( !rtems_chain_is_empty( &mount_chain ) ) {
      errno = EINVAL;
      goto cleanup_and_bail;
    }

    /*
     *  This is a mount of the base file system --> The
     *  mt_point_node.node_access will be left to null to indicate that this
     *  is the root of the entire file system.
     */
  }

  if ( (*mount_h)( mt_entry, data ) ) {
    /*
     * Try to undo the mount operation
     */
    loc.ops->unmount_h( mt_entry );
    goto cleanup_and_bail;
  }

  /*
   *  Add the mount table entry to the mount table chain
   */
  rtems_libio_lock();
  rtems_chain_append( &mount_chain, &mt_entry->Node );
  rtems_libio_unlock();

  if ( !has_target )
    rtems_filesystem_root = mt_entry->mt_fs_root;

  return 0;

cleanup_and_bail:

  free( mt_entry );

  if ( loc_to_free )
    rtems_filesystem_freenode( loc_to_free );

  return -1;
}

bool rtems_filesystem_mount_iterate(
  rtems_per_filesystem_mount_routine routine,
  void *routine_arg
)
{
  rtems_chain_node *node = NULL;
  bool stop = false;

  rtems_libio_lock();
  for (
    node = rtems_chain_first( &mount_chain );
    !rtems_chain_is_tail( &mount_chain, node ) && !stop;
    node = rtems_chain_next( node )
  ) {
    const rtems_filesystem_mount_table_entry_t *mt_entry =
      (rtems_filesystem_mount_table_entry_t *) node;

    stop = (*routine)( mt_entry, routine_arg );
  }
  rtems_libio_unlock();

  return stop;
}
