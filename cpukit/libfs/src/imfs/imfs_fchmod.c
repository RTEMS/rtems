/*
 *  IMFS file change mode routine.
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

int IMFS_fchmod(
  const rtems_filesystem_location_info_t *loc,
  mode_t mode
)
{
  IMFS_jnode_t  *jnode;
#if defined(RTEMS_POSIX_API)
  uid_t          st_uid;
#endif

  jnode = loc->node_access;

  /*
   *  Verify I am the owner of the node or the super user.
   */
#if defined(RTEMS_POSIX_API)
  st_uid = geteuid();

  if ( ( st_uid != jnode->st_uid ) && ( st_uid != 0 ) )
    rtems_set_errno_and_return_minus_one( EPERM );
#endif

  /*
   * Change only the RWX permissions on the jnode to mode.
   */

  jnode->st_mode &= ~(S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);
  jnode->st_mode |= mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);

  IMFS_update_ctime( jnode );

  return 0;
}
