/*
 *  IMFS_stat
 *  
 *  This routine provides a stat for the IMFS file system.
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

#include <errno.h>
#include "imfs.h"
#include <rtems/libio_.h>

int IMFS_stat(
  rtems_filesystem_location_info_t *loc,
  struct stat                      *buf
)
{
  IMFS_jnode_t   *the_jnode;
  IMFS_device_t  *io;

  the_jnode = loc->node_access;


  switch ( the_jnode->type ) {

    case IMFS_DEVICE:
      io          = &the_jnode->info.device;
      buf->st_dev = rtems_filesystem_make_dev_t( io->major, io->minor );
      break;

    case IMFS_LINEAR_FILE:
    case IMFS_MEMORY_FILE:
      buf->st_size = the_jnode->info.file.size;
      break;
   
    case IMFS_SYM_LINK:
      buf->st_size = 0;
      break;
   
    default:
      set_errno_and_return_minus_one( ENOTSUP );
      break;
  }

  buf->st_mode  = the_jnode->st_mode;
  buf->st_nlink = the_jnode->st_nlink;
  buf->st_ino   = the_jnode->st_ino;
  buf->st_uid   = the_jnode->st_uid;
  buf->st_gid   = the_jnode->st_gid;

  buf->st_atime = the_jnode->stat_atime;
  buf->st_mtime = the_jnode->stat_mtime;
  buf->st_ctime = the_jnode->stat_ctime;

  return 0;
}
