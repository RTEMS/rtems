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

#include <sys/types.h>
#include <sys/stat.h>
#include <chain.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "imfs.h"
#include "libio_.h"

/*
 *  Data structures and routines private to mount/unmount pair.
 */

extern Chain_Control                    rtems_filesystem_mount_table_control;
extern rtems_filesystem_location_info_t rtems_filesystem_current;

int search_mt_for_mount_point(
  rtems_filesystem_location_info_t *location_of_mount_point
);


int file_systems_below_this_mountpoint( 
  const char                            *mount_path,
  rtems_filesystem_location_info_t      *temp_loc,
  rtems_filesystem_mount_table_entry_t  *temp_mt_entry
);

/*
 *  unmount
 * 
 *  This routine will attempt to unmount the file system that has been
 *  is mounted a mount_path.  If the operation is successful, 0 will
 *  be returned to the calling routine.  Otherwise, 1 will be returned.
 */

int unmount(
  const char *mount_path
)
{
  int                                   status;
  rtems_filesystem_location_info_t      temp_loc;
  rtems_filesystem_mount_table_entry_t  temp_mt_entry;
  int                                   result;

  /*
   *  Are there any file systems below the mount_path specified
   */

  status = file_systems_below_this_mountpoint( 
    mount_path, 
    &temp_loc,
    &temp_mt_entry
  );

  if ( status != 0 )
    return -1;

  /*
   *  Is the current node reference pointing to a node in the file system
   *  we are attempting to unmount ?
   */

  if ( rtems_filesystem_current.mt_entry == temp_loc.mt_entry ) {
    rtems_filesystem_freenode( &temp_loc );
    set_errno_and_return_minus_one( EBUSY );
  }

  /*
   *  Run the file descriptor table to determine if there are any file
   *  descriptors that are currently active and reference nodes in the
   *  file system that we are trying to unmount 
   */

  if ( rtems_libio_is_open_files_in_fs( temp_loc.mt_entry ) == 1 ) {
    rtems_filesystem_freenode( &temp_loc );
    set_errno_and_return_minus_one( EBUSY );
  }
  
  /*
   * Allow the file system being mounted on to do its cleanup.
   * XXX - Did I change these correctly ??? It looks like either I did
   * XXX   this backwards or the IMFS_unmount and IMFS_fsumount are swaped.
   * XXX   Add to the mt_point_node unmount to set the mt_entry back to null
   * XXX   I will step off in space when evaluating past the end of the node.
   */

  if ((temp_mt_entry.mt_point_node.ops->unmount )( temp_loc.mt_entry ) != 0 ) {
    rtems_filesystem_freenode( &temp_loc );
    return -1;
  }

  /*
   * Run the unmount function for the subordinate file system.
   */

  if ((temp_mt_entry.mt_fs_root.ops->fsunmount_me )( temp_loc.mt_entry ) != 0){
    rtems_filesystem_freenode( &temp_loc );
    return -1;
  }

  /*
   * Allow the file system to clean up.
   */

  result = (*temp_loc.ops->fsunmount_me)( temp_loc.mt_entry );

  /*
   *  Extract the mount table entry from the chain
   */

  Chain_Extract( ( Chain_Node * ) temp_loc.mt_entry );

  /*
   *  Free the memory associated with the extracted mount table entry.
   */

  rtems_filesystem_freenode( &temp_loc.mt_entry->mt_point_node );
  free( temp_loc.mt_entry );
  rtems_filesystem_freenode( &temp_loc );

  return result;

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
  const char                            *mount_path,
  rtems_filesystem_location_info_t      *temp_loc,
  rtems_filesystem_mount_table_entry_t  *temp_mt_entry
)
{
  Chain_Node                           *the_node;
  rtems_filesystem_mount_table_entry_t *the_mount_entry;
  rtems_filesystem_mount_table_entry_t *current_fs_mt_entry;

  /*
   *  Is the mount_path even a valid node name in the existing tree?
   */

  if ( rtems_filesystem_evaluate_path( mount_path, 0x0, temp_loc, TRUE ) )
    return -1;
  
  /*
   *  Look for the node defined in temp_loc as a mount point in the
   *  mount table chain.
   */

  for ( the_node = rtems_filesystem_mount_table_control.first;
        !Chain_Is_tail( &rtems_filesystem_mount_table_control, the_node );
        the_node = the_node->next ) {

     the_mount_entry = ( rtems_filesystem_mount_table_entry_t * )the_node;
     if  (the_mount_entry->mt_point_node.node_access  ==
          temp_loc->node_access ) {
        current_fs_mt_entry = the_mount_entry;
        *temp_loc = current_fs_mt_entry->mt_fs_root;
        goto after_real_mount_point_found;
     }
  }
  set_errno_and_return_minus_one( EACCES );


after_real_mount_point_found:

  for ( the_node = rtems_filesystem_mount_table_control.first;
        !Chain_Is_tail( &rtems_filesystem_mount_table_control, the_node );
        the_node = the_node->next ) {

     the_mount_entry = (  rtems_filesystem_mount_table_entry_t * )the_node;

     /*
      *  Exclude the current file systems mount table entry from the test
      */

     if ( current_fs_mt_entry != the_mount_entry ) {
       if ( the_mount_entry->mt_point_node.mt_entry  == current_fs_mt_entry ) {

          /*
           *  There is at least one fs below the fs on the mount_path.
           */
          set_errno_and_return_minus_one( EBUSY );
       }
     }
  }

  *temp_mt_entry = *current_fs_mt_entry;

  return 0;
}
