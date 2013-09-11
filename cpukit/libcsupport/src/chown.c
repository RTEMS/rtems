/**
 *  @file
 *
 *  @brief Change Owner and Group of a File
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <unistd.h>

#include <rtems/libio_.h>

/**
 *  POSIX 1003.1b 5.6.5 - Change Owner and Group of a File
 */
int chown( const char *path, uid_t owner, gid_t group )
{
  int rv;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_LINK;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  rv = rtems_filesystem_chown( currentloc, owner, group );

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
