/*
 *  unmount() - Unmount a File System
 *
 *  This routine is not defined in the POSIX 1003.1b standard but 
 *  in some form is supported on most UNIX and POSIX systems.  This
 *  routine is necessary to mount instantiations of a file system
 *  into the file system name space.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <chain.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

/*
 *  Data structures and routines private to mount/unmount pair.
 */

extern Chain_Control                    rtems_filesystem_mount_table_control;

int search_mt_for_mount_point(
  rtems_filesystem_location_info_t *location_of_mount_point
);

rtems_boolean rtems_filesystem_nodes_equal( 
  const rtems_filesystem_location_info_t   *loc1,
  const rtems_filesystem_location_info_t   *loc2
){
  return ( loc1->node_access == loc2->node_access );
}


/*
 *  file_systems_below_this_mountpoint
 *
 *  This routine will run through the entries that currently exist in the
 *  mount table chain. For each entry in the mount table chain it will
 *  compare the mount tables mt_fs_root to the new_fs_root_node. If any of the
 *  mount table file system root nodes matches the new file system root node
 *  this indicates that we are trying to mount a file system that has already
 *  been mounted. This is not a permitted operation. temp_loc is set to 
 *  the root node of the file system being unmounted.
 */

rtems_boolean file_systems_below_this_mountpoint( 
  const char                            *path,
  rtems_filesystem_location_info_t      *fs_root_loc,
  rtems_filesystem_mount_table_entry_t  *fs_to_unmount
)
{
  Chain_Node                           *the_node;
  rtems_filesystem_mount_table_entry_t *the_mount_entry;

  /*
   * Search the mount table for any mount entries referencing this
   * mount entry.
   */

  for ( the_node = rtems_filesystem_mount_table_control.first;
        !Chain_Is_tail( &rtems_filesystem_mount_table_control, the_node );
        the_node = the_node->next ) {
     the_mount_entry = ( rtems_filesystem_mount_table_entry_t * )the_node;
     if (the_mount_entry->mt_point_node.mt_entry  == fs_root_loc->mt_entry ) {
        return TRUE;
     }
  }

  return FALSE;
}

/*
 *  unmount
 * 
 *  This routine will attempt to unmount the file system that has been
 *  is mounted a path.  If the operation is successful, 0 will
 *  be returned to the calling routine.  Otherwise, 1 will be returned.
 */

int unmount(
  const char *path
)
{
  rtems_filesystem_location_info_t      loc;
  rtems_filesystem_location_info_t     *fs_root_loc;
  rtems_filesystem_location_info_t     *fs_mount_loc;
  rtems_filesystem_mount_table_entry_t *mt_entry;

  /*
   *  Get
   *    The root node of the mounted filesytem.
   *    The node for the directory that the fileystem is mounted on.
   *    The mount entry that is being refered to.
   */

  if ( rtems_filesystem_evaluate_path( path, 0x0, &loc, TRUE ) )
    return -1;

  mt_entry     = loc.mt_entry;
  fs_mount_loc = &mt_entry->mt_point_node;
  fs_root_loc  = &mt_entry->mt_fs_root;
	
  /*
   * Verify this is the root node for the file system to be unmounted.
   */

  if ( !rtems_filesystem_nodes_equal( fs_root_loc, &loc) ){
    rtems_filesystem_freenode( &loc );
    rtems_set_errno_and_return_minus_one( EACCES );
  }

  /*
   * Free the loc node and just use the nodes from the mt_entry .
   */

  rtems_filesystem_freenode( &loc );

  /*
   * Verify Unmount is supported by both filesystems.
   */

  if ( !fs_mount_loc->ops->unmount_h )
    rtems_set_errno_and_return_minus_one( ENOTSUP );

  if ( !fs_root_loc->ops->fsunmount_me_h )
    rtems_set_errno_and_return_minus_one( ENOTSUP );


  /*
   *  Verify the current node is not in this filesystem.
   *  XXX - Joel I have a question here wasn't code added
   *        that made the current node thread based instead
   *        of system based?  I thought it was but it doesn't
   *        look like it in this version.
   */

  if ( rtems_filesystem_current.mt_entry == mt_entry )
    rtems_set_errno_and_return_minus_one( EBUSY );

  /*
   *  Verify there are no file systems below the path specified
   */

  if ( file_systems_below_this_mountpoint( path, fs_root_loc, mt_entry ) != 0 )
    rtems_set_errno_and_return_minus_one( EBUSY );

  /*
   *  Run the file descriptor table to determine if there are any file
   *  descriptors that are currently active and reference nodes in the
   *  file system that we are trying to unmount 
   */

  if ( rtems_libio_is_open_files_in_fs( mt_entry ) == 1 ) 
    rtems_set_errno_and_return_minus_one( EBUSY );
  
  /*
   * Allow the file system being unmounted on to do its cleanup.
   * If it fails it will set the errno to the approprate value
   * and the fileystem will not be modified.
   */

  if (( fs_mount_loc->ops->unmount_h )( mt_entry ) != 0 )
    return -1;

  /*
   *  Allow the mounted filesystem to unmark the use of the root node.
   *
   *  Run the unmount function for the subordinate file system.
   *
   *  If we fail to unmount the filesystem remount it on the base filesystems
   *  directory node.
   *
   *  NOTE:  Fatal error is called in a case which should never happen
   *         This was response was questionable but the best we could 
   *         come up with.
   */

  if ((fs_root_loc->ops->fsunmount_me_h )( mt_entry ) != 0){
    if (( fs_mount_loc->ops->mount_h )( mt_entry ) != 0 )
      rtems_fatal_error_occurred( 0 );
    return -1;
  }

  /*
   *  Extract the mount table entry from the chain
   */

  Chain_Extract( ( Chain_Node * ) mt_entry );

  /*
   *  Free the memory node that was allocated in mount
   *  Free the memory associated with the extracted mount table entry.
   */

  rtems_filesystem_freenode( fs_mount_loc );
  free( mt_entry );

  return 0;
}


