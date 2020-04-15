/**
 * @file
 *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/imfs.h>

#include <sys/param.h>
#include <dirent.h>
#include <string.h>

static ssize_t IMFS_dir_read(
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
         dir_ent->d_ino = IMFS_node_to_ino( imfs_node );
#ifdef DT_DIR
         dir_ent->d_type = IFTODT( imfs_node->st_mode );
#endif
         dir_ent->d_namlen =
           MIN( imfs_node->namelen, sizeof( dir_ent->d_name ) - 1 );
         dir_ent->d_name[ dir_ent->d_namlen ] = '\0';
         memcpy( dir_ent->d_name, imfs_node->name, dir_ent->d_namlen );

         iop->offset += sizeof( *dir_ent );
         bytes_transferred += (ssize_t) sizeof( *dir_ent );
      }
   }

   rtems_filesystem_instance_unlock( &iop->pathinfo );

   return bytes_transferred;
}

static size_t IMFS_directory_size( const IMFS_jnode_t *node )
{
  size_t size = 0;
  const IMFS_directory_t *dir = (const IMFS_directory_t *) node;
  const rtems_chain_control *chain = &dir->Entries;
  const rtems_chain_node *current = rtems_chain_immutable_first( chain );
  const rtems_chain_node *tail = rtems_chain_immutable_tail( chain );

  while ( current != tail ) {
    size += sizeof( struct dirent );
    current = rtems_chain_immutable_next( current );
  }

  return size;
}

static int IMFS_stat_directory(
  const rtems_filesystem_location_info_t *loc,
  struct stat *buf
)
{
  const IMFS_jnode_t *node = loc->node_access;

  buf->st_size = IMFS_directory_size( node );

  return IMFS_stat( loc, buf );
}

static const rtems_filesystem_file_handlers_r IMFS_dir_default_handlers = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = IMFS_dir_read,
  .write_h = rtems_filesystem_default_write,
  .ioctl_h = rtems_filesystem_default_ioctl,
  .lseek_h = rtems_filesystem_default_lseek_directory,
  .fstat_h = IMFS_stat_directory,
  .ftruncate_h = rtems_filesystem_default_ftruncate_directory,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync_success,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

const IMFS_mknod_control IMFS_mknod_control_dir_default = {
  {
    .handlers = &IMFS_dir_default_handlers,
    .node_initialize = IMFS_node_initialize_directory,
    .node_remove = IMFS_node_remove_directory,
    .node_destroy = IMFS_node_destroy_default
  },
  .node_size = sizeof( IMFS_directory_t )
};
