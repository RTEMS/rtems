/**
 * @file
 *
 * @brief Change IMFS File Mode
 * @ingroup IMFS
 */

/*
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

  jnode = loc->node_access;

  jnode->st_mode = mode;

  IMFS_update_ctime( jnode );

  return 0;
}
