/*
 *  IMFS_utime
 *
 *  This routine is the implementation of the utime() system 
 *  call for the IMFS.
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

#include <errno.h>
#include <sys/time.h>

#include <rtems/libio_.h>
#include "imfs.h"

int IMFS_utime(
  rtems_filesystem_location_info_t  *pathloc,       /* IN */
  time_t                             actime,        /* IN */
  time_t                             modtime        /* IN */
)
{
  IMFS_jnode_t *the_jnode;

  the_jnode = (IMFS_jnode_t *) pathloc->node_access;

  the_jnode->stat_atime = actime;
  the_jnode->stat_mtime = modtime;

  return 0;
}
