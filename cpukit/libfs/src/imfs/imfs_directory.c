/**
 * @file
 *
 * @brief IMFS Read Next Directory
 * @ingroup IMFS
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

#include <string.h>
#include <dirent.h>

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
   const IMFS_directory_t    *dir;
   const rtems_chain_node    *node;
   const rtems_chain_control *entries;
   struct dirent             *dir_ent;
   ssize_t                    bytes_transferred;
   off_t                      current_entry;
   off_t                      first_entry;
   off_t                      last_entry;

   rtems_filesystem_instance_lock( &iop->pathinfo );

   dir = IMFS_iop_to_directory( iop );
   entries = &dir->Entries;

   /* Move to the first of the desired directory entries */

   bytes_transferred = 0;
   first_entry = iop->offset;
   /* protect against using sizes that are not exact multiples of the */
   /* -dirent- size. These could result in unexpected results          */
   last_entry = first_entry
     + (count / sizeof( *dir_ent )) * sizeof( *dir_ent );

   /* The directory was not empty so try to move to the desired entry in chain*/
   for (
      current_entry = 0,
        node = rtems_chain_immutable_first( entries );
      current_entry < last_entry
        && !rtems_chain_is_tail( entries, node );
      current_entry +=  sizeof( *dir_ent ),
        node = rtems_chain_immutable_next( node )
   ) {
      if( current_entry >= first_entry ) {
         const IMFS_jnode_t *imfs_node = (const IMFS_jnode_t *) node;

         dir_ent = (struct dirent *) ((char *) buffer + bytes_transferred);

         /* Move the entry to the return buffer */
         dir_ent->d_off = current_entry;
         dir_ent->d_reclen = sizeof( *dir_ent );
         dir_ent->d_ino = imfs_node->st_ino;
         dir_ent->d_namlen = strlen( imfs_node->name );
         memcpy( dir_ent->d_name, imfs_node->name, dir_ent->d_namlen + 1 );

         iop->offset += sizeof( *dir_ent );
         bytes_transferred += (ssize_t) sizeof( *dir_ent );
      }
   }

   rtems_filesystem_instance_unlock( &iop->pathinfo );

   return bytes_transferred;
}
