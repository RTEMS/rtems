/*
 *  IMFS_chown
 *
 *  This routine is the implementation of the chown() system
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

#include <unistd.h>

#include <rtems/libio_.h>

int IMFS_chown(
  const rtems_filesystem_location_info_t *loc,
  uid_t owner,
  gid_t group
)
{
  IMFS_jnode_t  *jnode;
#if defined(RTEMS_POSIX_API)
  uid_t          st_uid;
#endif

  jnode = (IMFS_jnode_t *) loc->node_access;

  /*
   *  Verify I am the owner of the node or the super user.
   */

#if defined(RTEMS_POSIX_API)
  st_uid = geteuid();

  if ( ( st_uid != jnode->st_uid ) && ( st_uid != 0 ) )
    rtems_set_errno_and_return_minus_one( EPERM );
#endif

  jnode->st_uid = owner;
  jnode->st_gid = group;

  IMFS_update_ctime( jnode );

  return 0;
}
