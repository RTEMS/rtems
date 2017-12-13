/**
 * @file
 *
 * @brief IMFS Change Owner
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

#include <rtems/imfs.h>

#include <unistd.h>

#include <rtems/libio_.h>

int IMFS_chown(
  const rtems_filesystem_location_info_t *loc,
  uid_t owner,
  gid_t group
)
{
  IMFS_jnode_t *jnode;

  jnode = (IMFS_jnode_t *) loc->node_access;

  jnode->st_uid = owner;
  jnode->st_gid = group;

  IMFS_update_ctime( jnode );

  return 0;
}
