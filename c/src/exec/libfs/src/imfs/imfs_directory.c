/*
 *  IMFS Directory Access Routines
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
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>

#include "imfs.h"
#include <rtems/libio_.h>

/*
 *  imfs_dir_open
 *
 *  This rountine will verify that the node being opened as a directory is
 *  in fact a directory node. If it is then the offset into the directory 
 *  will be set to 0 to position to the first directory entry.
 */

int imfs_dir_open( 
  rtems_libio_t  *iop,
  const char *pathname,
  unsigned32 flag,
  unsigned32 mode
)
{
  IMFS_jnode_t      *the_jnode;

  /* Is the node a directory ? */
  the_jnode = (IMFS_jnode_t *) iop->file_info;

  if ( the_jnode->type != IMFS_DIRECTORY )
     return -1;      /* It wasn't a directory --> return error */

  iop->offset = 0;
  return 0;
}

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

int imfs_dir_read(
  rtems_libio_t  *iop,
  void *buffer,
  unsigned32 count
)
{
  /*
   *  Read up to element  iop->offset in the directory chain of the
   *  imfs_jnode_t struct for this file descriptor.
   */
   Chain_Node        *the_node;
   Chain_Control     *the_chain;
   IMFS_jnode_t      *the_jnode;
   int                bytes_transferred;
   int                current_entry;
   int                first_entry;
   int                last_entry;
   struct dirent      tmp_dirent;

   the_jnode = (IMFS_jnode_t *)iop->file_info;
   the_chain = &the_jnode->info.directory.Entries;
   
   if ( Chain_Is_empty( the_chain ) )
      return 0;

   /* Move to the first of the desired directory entries */
   the_node = the_chain->first;

   bytes_transferred = 0;
   first_entry = iop->offset;
   /* protect against using sizes that are not exact multiples of the */
   /* -dirent- size. These could result in unexpected results          */
   last_entry = first_entry + (count/sizeof(struct dirent)) * sizeof(struct dirent);

   /* The directory was not empty so try to move to the desired entry in chain*/
   for ( 
      current_entry = 0; 
      current_entry < last_entry; 
      current_entry = current_entry + sizeof(struct dirent) ){

      if ( Chain_Is_tail( the_chain, the_node ) ){ 
         /* We hit the tail of the chain while trying to move to the first */
         /* entry in the read */
         return bytes_transferred;  /* Indicate that there are no more */
                                    /* entries to return */
      }
    
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
         iop->offset = iop->offset + sizeof(struct dirent);
         bytes_transferred = bytes_transferred + sizeof( struct dirent );
      }

      the_node = the_node->next;
   }

   /* Success */
   return bytes_transferred;
}



/*
 *  imfs_dir_close
 *
 *  This routine will be called by the generic close routine to cleanup any
 *  resources that have been allocated for the management of the file
 */

int imfs_dir_close(
  rtems_libio_t  *iop
)
{
  /*
   *  The generic close routine handles the deallocation of the file control
   *  and associated memory. At present the imfs_dir_close simply
   *  returns a successful completion status.
   */
 
  return 0;
}



/*
 *  imfs_dir_lseek
 *
 *  This routine will behave in one of three ways based on the state of 
 *  argument whence. Based on the state of its value the offset argument will
 *  be interpreted using one of the following methods:
 *
 *     SEEK_SET - offset is the absolute byte offset from the start of the
 *                logical start of the dirent sequence that represents the
 *                directory
 *     SEEK_CUR - offset is used as the relative byte offset from the current
 *                directory position index held in the iop structure
 *     SEEK_END - N/A --> This will cause an assert.
 */

int imfs_dir_lseek(
  rtems_libio_t  *iop,
  off_t           offset,
  int             whence
)
{
  switch( whence ) {
     case SEEK_SET:   /* absolute move from the start of the file */
     case SEEK_CUR:   /* relative move */
        iop->offset = (iop->offset/sizeof(struct dirent)) *
              sizeof(struct dirent);
        break;

     case SEEK_END:   /* Movement past the end of the directory via lseek */
                      /* is not a permitted operation                     */
      default:
        set_errno_and_return_minus_one( EINVAL );
        break;
  }

  return 0;
}



/*
 *  imfs_dir_fstat
 *
 *  This routine will obtain the following information concerning the current 
 *  directory:
 *        st_dev      0ll
 *        st_ino      1
 *        st_mode     mode extracted from the jnode
 *        st_nlink    number of links to this node
 *        st_uid      uid extracted from the jnode
 *        st_gid      gid extracted from the jnode
 *        st_rdev     0ll
 *        st_size     the number of bytes in the directory 
 *                    This is calculated by taking the number of entries
 *                    in the directory and multiplying by the size of a 
 *                    dirent structure
 *        st_blksize  0
 *        st_blocks   0
 *        stat_atime  time of last access
 *        stat_mtime  time of last modification
 *        stat_ctime  time of the last change
 *
 *  This information will be returned to the calling function in a -stat- struct
 *
 */

int imfs_dir_fstat(
  rtems_filesystem_location_info_t *loc,
  struct stat                      *buf
)
{
   Chain_Node        *the_node;
   Chain_Control     *the_chain;
   IMFS_jnode_t      *the_jnode;


   the_jnode = (IMFS_jnode_t *) loc->node_access;

   buf->st_dev = 0ll;
   buf->st_ino   = the_jnode->st_ino;
   buf->st_mode  = the_jnode->st_mode;
   buf->st_nlink = the_jnode->st_nlink;
   buf->st_uid   = the_jnode->st_uid;
   buf->st_gid   = the_jnode->st_gid;
   buf->st_rdev = 0ll;
   buf->st_blksize = 0;
   buf->st_blocks = 0;
   buf->st_atime = the_jnode->stat_atime;
   buf->st_mtime = the_jnode->stat_mtime;
   buf->st_ctime = the_jnode->stat_ctime;

   buf->st_size = 0;

   the_chain = &the_jnode->info.directory.Entries;

   /* Run through the chain and count the number of directory entries */
   /* that are subordinate to this directory node                     */
   for ( the_node = the_chain->first ;
         !_Chain_Is_tail( the_chain, the_node ) ;
         the_node = the_node->next ) {
 
      buf->st_size = buf->st_size + sizeof( struct dirent );
   }

   return 0;
}

/*
 *  IMFS_dir_rmnod
 *
 *  This routine is available from the optable to remove a node 
 *  from the IMFS file system.
 */

int imfs_dir_rmnod(
  rtems_filesystem_location_info_t      *pathloc       /* IN */
)
{
  IMFS_jnode_t *the_jnode;  

  the_jnode = (IMFS_jnode_t *) pathloc->node_access;

  /* 
   * You cannot remove a node that still has children 
   */

  if ( ! Chain_Is_empty( &the_jnode->info.directory.Entries ) )
     set_errno_and_return_minus_one( ENOTEMPTY );

  /* 
   * You cannot remove the file system root node.
   */

  if ( pathloc->mt_entry->mt_fs_root.node_access == pathloc->node_access )
     set_errno_and_return_minus_one( EBUSY );

  /* 
   * You cannot remove a mountpoint.
   */

   if ( the_jnode->info.directory.mt_fs != NULL )
     set_errno_and_return_minus_one( EBUSY );          
 
  /* 
   * Take the node out of the parent's chain that contains this node 
   */

  if ( the_jnode->Parent != NULL ) {
    Chain_Extract( (Chain_Node *) the_jnode );
    the_jnode->Parent = NULL;
  }

  /*
   * Decrement the link counter and see if we can free the space.
   */

  the_jnode->st_nlink--;
  IMFS_update_ctime( the_jnode );

  /*
   * The file cannot be open and the link must be less than 1 to free.
   */

  if ( !rtems_libio_is_file_open( the_jnode ) && (the_jnode->st_nlink < 1) ) {

    /* 
     * Is the rtems_filesystem_current is this node?
     */

    if ( rtems_filesystem_current.node_access == pathloc->node_access )
       rtems_filesystem_current.node_access = NULL;

    /*
     * Free memory associated with a memory file.
     */

    free( the_jnode );
  }

  return 0;

}


