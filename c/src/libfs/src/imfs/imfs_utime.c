/*
 *  IMFS_utime
 *
 *  This routine is the implementation of the utime() system 
 *  call for the IMFS.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <errno.h>
#include <sys/time.h>

#include "libio_.h"
#include "imfs.h"

int IMFS_utime(
  rtems_filesystem_location_info_t  *pathloc,       /* IN */
  time_t                             actime,        /* IN */
  time_t                             modtime        /* IN */
)
{
  IMFS_jnode_t *the_jnode;

  the_jnode = (IMFS_jnode_t *) pathloc->node_access;

  the_jnode->st_atime = actime;
  the_jnode->st_mtime = modtime;

  return 0;
}
