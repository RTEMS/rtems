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

/*
 *  Data structures and routines private to mount/unmount pair.
 */

extern Chain_Control                    rtems_filesystem_mount_table_control;
extern rtems_filesystem_location_info_t rtems_filesystem_current;

int search_mt_for_mount_point(
  rtems_filesystem_location_info_t *location_of_mount_point
);


int file_systems_below_this_mountpoint( 
  const char                            *path,
  rtems_filesystem_location_info_t      *temp_loc,
  rtems_filesystem_mount_table_entry_t  *temp_mt_entry
);

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
  int                                   status;
  rtems_filesystem_location_info_t      fs_root_loc;
  rtems_filesystem_mount_table_entry_t  temp_mt_entry;

  /*
   *  Are there any file systems below the path specified
   */

  status = file_systems_below_this_mountpoint( 
    path, 
    &fs_root_loc,
    &temp_mt_entry
  );

  if ( status != 0 )
    return -1;

  /*
   *  Is the current node reference pointing to a node in the file system
   *  we are attempting to unmount ?
   */

  if ( rtems_filesystem_current.mt_entry == fs_root_loc.mt_entry ) {
    rtems_filesystem_freenode( &fs_root_loc );
    set_errno_and_return_minus_one( EBUSY );
  }

  /*
   *  Run the file descriptor table to determine if there are any file
   *  descriptors that are currently active and reference nodes in the
   *  file system that we are trying to unmount 
   */

  if ( rtems_libio_is_open_files_in_fs( fs_root_loc.mt_entry ) == 1 ) {
    rtems_filesystem_freenode( &fs_root_loc );
    set_errno_and_return_minus_one( EBUSY );
  }
  
  /*
   * Allow the file system being unmounted on to do its cleanup.
   */

  if ((temp_mt_entry.mt_point_node.ops->unmount_h )( fs_root_loc.mt_entry ) != 0 ) {
    rtems_filesystem_freenode( &fs_root_loc );
    return -1;
  }

  /*
   * Run the unmount function for the subordinate file system.
   */

  if ((temp_mt_entry.mt_fs_root.ops->fsunmount_me_h )( fs_root_loc.mt_entry ) != 0){
    rtems_filesystem_freenode( &fs_root_loc );
    return -1;
  }

  /*
   *  Extract the mount table entry from the chain
   */

  Chain_Extract( ( Chain_Node * ) fs_root_loc.mt_entry );

  /*
   *  Free the memory associated with the extracted mount table entry.
   */

  rtems_filesystem_freenode( &fs_root_loc.mt_entry->mt_point_node );
  free( fs_root_loc.mt_entry );
  rtems_filesystem_freenode( &fs_root_loc );

  return 0;
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

int file_systems_below_this_mountpoint( 
  const char                            *path,
  rtems_filesystem_location_info_t      *fs_root_loc,
  rtems_filesystem_mount_table_entry_t  *fs_to_unmount
)
{
  Chain_Node                           *the_node;
  rtems_filesystem_mount_table_entry_t *the_mount_entry;

  /*
   *  Is the path even a valid node name in the existing tree?
   */

  if ( rtems_filesystem_evaluate_path( path, 0x0, fs_root_loc, TRUE ) )
    return -1;
  
  /*
   * Verify this is the root node for the file system to be unmounted.
   */

  *fs_to_unmount = *fs_root_loc->mt_entry;
  if ( fs_to_unmount->mt_fs_root.node_access != fs_root_loc->node_access )
    set_errno_and_return_minus_one( EACCES );

  /*
   * Search the mount table for any mount entries referencing this
   * mount entry.
   */

  for ( the_node = rtems_filesystem_mount_table_control.first;
        !Chain_Is_tail( &rtems_filesystem_mount_table_control, the_node );
        the_node = the_node->next ) {
     the_mount_entry = ( rtems_filesystem_mount_table_entry_t * )the_node;
     if (the_mount_entry->mt_point_node.mt_entry  == fs_root_loc->mt_entry ) {
          set_errno_and_return_minus_one( EBUSY );
     }
  }

  return 0;
}
