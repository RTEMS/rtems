/*
 *  IMFS Directory Access Routines
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

#include <string.h>
#include <dirent.h>

/*
 *  imfs_dir_read
 *
 *  This routine will read the next directory entry based on the directory
 *  offset. The offset should be equal to -n- time the size of an individual
 *  dirent structure. If n is not an integer multiple of the sizeof a
 *  dirent structure, an integer division will be performed to determine
 *  directory entry that will be returned in the buffer. Count should reflect
 *  -m- times the sizeof dirent bytes to be placed in the buffer.
 *  If there are not -m- dirent elements from the current directory position
 *  to the end of the exisiting file, the remaining entries will be placed in
 *  the buffer and the returned value will be equal to -m actual- times the
 *  size of a directory entry.
 */

ssize_t imfs_dir_read(
  rtems_libio_t  *iop,
  void           *buffer,
  size_t          count
)
{
  /*
   *  Read up to element  iop->offset in the directory chain of the
   *  imfs_jnode_t struct for this file descriptor.
   */
   rtems_chain_node    *the_node;
   rtems_chain_control *the_chain;
   IMFS_jnode_t        *the_jnode;
   int                  bytes_transferred;
   int                  current_entry;
   int                  first_entry;
   int                  last_entry;
   struct dirent        tmp_dirent;

   rtems_filesystem_instance_lock( &iop->pathinfo );

   the_jnode = (IMFS_jnode_t *)iop->pathinfo.node_access;
   the_chain = &the_jnode->info.directory.Entries;

   /* Move to the first of the desired directory entries */

   bytes_transferred = 0;
   first_entry = iop->offset;
   /* protect against using sizes that are not exact multiples of the */
   /* -dirent- size. These could result in unexpected results          */
   last_entry = first_entry
     + (count / sizeof( struct dirent )) * sizeof( struct dirent );

   /* The directory was not empty so try to move to the desired entry in chain*/
   for (
      current_entry = 0,
        the_node = rtems_chain_first( the_chain );
      current_entry < last_entry
        && !rtems_chain_is_tail( the_chain, the_node );
      current_entry +=  sizeof( struct dirent ),
        the_node = rtems_chain_next( the_node )
   ) {
      if( current_entry >= first_entry ) {
         /* Move the entry to the return buffer */
         tmp_dirent.d_off = current_entry;
         tmp_dirent.d_reclen = sizeof( struct dirent );
         the_jnode = (IMFS_jnode_t *) the_node;
         tmp_dirent.d_ino = the_jnode->st_ino;
         tmp_dirent.d_namlen = strlen( the_jnode->name );
         strcpy( tmp_dirent.d_name, the_jnode->name );
         memcpy(
            buffer + bytes_transferred,
            (void *)&tmp_dirent,
            sizeof( struct dirent )
         );
         iop->offset += sizeof( struct dirent );
         bytes_transferred += sizeof( struct dirent );
      }
   }

   rtems_filesystem_instance_unlock( &iop->pathinfo );

   return bytes_transferred;
}
