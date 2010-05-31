/*
 *  mount()
 *
 *  XXX
 *
 *  XXX make sure no required ops are NULL
 *  XXX make sure no optional ops you are using are NULL
 *  XXX unmount should be required.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <rtems/libio_.h>

/*
 * External defined by confdefs.h or the user.
 */
extern const rtems_filesystem_table_t configuration_filesystem_table[];

/*
 * Points to a list of filesystems added at runtime.
 */
rtems_chain_control *rtems_filesystem_table;

/*
 * Mount table list.
 */
rtems_chain_control rtems_filesystem_mount_table_control;
bool                rtems_filesystem_mount_table_control_init;

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

/*
 *  Is_node_fs_root
 *
 *  This routine will run through the entries that currently exist in the
 *  mount table chain. For each entry in the mount table chain it will
 *  compare the mount tables root node to the node describing the selected
 *  mount point. If any match is found true is returned else false is
 *  returned.
 *
 */

static bool Is_node_fs_root(
  rtems_filesystem_location_info_t  *loc
)
{
  rtems_chain_node                     *the_node;
  rtems_filesystem_mount_table_entry_t *the_mount_entry;

  /*
   * For each mount table entry
   */
  if ( rtems_filesystem_mount_table_control_init ) {
    for ( the_node = rtems_chain_first( &rtems_filesystem_mount_table_control );
          !rtems_chain_is_tail( &rtems_filesystem_mount_table_control, the_node );
          the_node = rtems_chain_next( the_node ) ) {
      the_mount_entry = (rtems_filesystem_mount_table_entry_t *) the_node;
      if ( the_mount_entry->mt_fs_root.node_access  == loc->node_access )
        return true;
    }
  }
  return false;
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
  const rtems_filesystem_table_t       *entry;
  rtems_filesystem_location_info_t      loc;
  rtems_filesystem_mount_table_entry_t *mt_entry = NULL;
  rtems_filesystem_location_info_t     *loc_to_free = NULL;
  size_t size;

  /*
   * If mount is ever called we allocate the mount table control structure.
   */
  if ( !rtems_filesystem_mount_table_control_init ) {
    rtems_filesystem_mount_table_control_init = true;
    rtems_chain_initialize_empty ( &rtems_filesystem_mount_table_control );
  }

  /*
   *  Are the file system options valid?
   */

  if ( options != RTEMS_FILESYSTEM_READ_ONLY &&
       options != RTEMS_FILESYSTEM_READ_WRITE ) {
    errno = EINVAL;
    return -1;
  }

  /*
   * Check the type.
   */
  if (!filesystemtype) {
    errno = EINVAL;
    return -1;
  }

  if (strlen(filesystemtype) >= 128) {
    errno = EINVAL;
    return -1;
  }
    
  /*
   * Check the configuration table filesystems then check any runtime added
   * file systems.
   */
  entry = &configuration_filesystem_table[0];
  while (entry->type) {
    if (strcmp (filesystemtype, entry->type) == 0)
      break;
    ++entry;
  }
  
  if (!entry->type) {
    entry = NULL;
    if (rtems_filesystem_table) {
      rtems_chain_node *the_node;
      for (the_node = rtems_chain_first(rtems_filesystem_table);
           !rtems_chain_is_tail(rtems_filesystem_table, the_node);
           the_node = rtems_chain_next(the_node)) {
        entry = &(((rtems_filesystem_table_node_t*) the_node)->entry);
        if (strcmp (filesystemtype, entry->type) == 0)
          break;
        entry = NULL;
      }
    }
  }

  if (!entry)
  {
    errno = EINVAL;
    return -1;
  }
  
  /*
   * Allocate a mount table entry
   */

  size = sizeof(rtems_filesystem_mount_table_entry_t);
  if ( source )
    size += strlen( source ) + 1;
   
  mt_entry = malloc( size );
  if ( !mt_entry ) {
    errno = ENOMEM;
    return -1;
  }

  memset( mt_entry, 0, size );
   
  mt_entry->mt_fs_root.mt_entry = mt_entry;
  mt_entry->type = entry->type;
  mt_entry->options = options;
  mt_entry->pathconf_limits_and_options = rtems_filesystem_default_pathconf;
   
  if ( source ) {
    mt_entry->dev =
      (char *)mt_entry + sizeof( rtems_filesystem_mount_table_entry_t );
    strcpy( mt_entry->dev, source );
  } else
    mt_entry->dev = 0;

  /*
   *  The mount_point should be a directory with read/write/execute
   *  permissions in the existing tree.
   */

  if ( target ) {

    if ( rtems_filesystem_evaluate_path(
           target, strlen( target ), RTEMS_LIBIO_PERMS_RWX, &loc, true ) == -1 )
      goto cleanup_and_bail;

    loc_to_free = &loc;

    /*
     * Test for node_type_h
     */

    if (!loc.ops->node_type_h) {
      errno =  ENOTSUP;
      goto cleanup_and_bail;
    }

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

    if ( Is_node_fs_root(  &loc ) ){
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

    if ( !loc.ops->mount_h ){
      errno = ENOTSUP;
      goto cleanup_and_bail;
    }

    if ( loc.ops->mount_h( mt_entry ) ) {
      goto cleanup_and_bail;
    }

    mt_entry->target = strdup( target );
  } else {

    /*
     * Do we already have a base file system ?
     */
    if ( !rtems_chain_is_empty( &rtems_filesystem_mount_table_control ) ) {
      errno = EINVAL;
      goto cleanup_and_bail;
    }
    
    /*
     *  This is a mount of the base file system --> The
     *  mt_point_node.node_access will be set to null to indicate that this
     *  is the root of the entire file system.
     */

    mt_entry->mt_fs_root.node_access = NULL;
    mt_entry->mt_fs_root.handlers = NULL;
    mt_entry->mt_fs_root.ops = NULL;

    mt_entry->mt_point_node.node_access = NULL;
    mt_entry->mt_point_node.handlers = NULL;
    mt_entry->mt_point_node.ops = NULL;
    mt_entry->mt_point_node.mt_entry = NULL;

    mt_entry->target = "/";
  }

  if ( entry->mount_h( mt_entry, data ) ) {
    /*
     * Try to undo the mount operation
     */
    if ( loc.ops->unmount_h ) {
      loc.ops->unmount_h( mt_entry );
    }
    goto cleanup_and_bail;
  }

  /*
   *  Add the mount table entry to the mount table chain
   */
  rtems_chain_append( &rtems_filesystem_mount_table_control,
                      &mt_entry->Node );

  if ( !target )
    rtems_filesystem_root = mt_entry->mt_fs_root;

  return 0;

cleanup_and_bail:

  free( (void*) mt_entry->target );
  free( mt_entry );

  if ( loc_to_free )
    rtems_filesystem_freenode( loc_to_free );

  return -1;
}

