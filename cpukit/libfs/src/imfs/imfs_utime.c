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
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

#include <sys/time.h>

int IMFS_utime(
  const rtems_filesystem_location_info_t *loc,
  time_t actime,
  time_t modtime
)
{
  IMFS_jnode_t *the_jnode;

  the_jnode = (IMFS_jnode_t *) loc->node_access;

  the_jnode->stat_atime = actime;
  the_jnode->stat_mtime = modtime;
  the_jnode->stat_ctime = time( NULL );

  return 0;
}
