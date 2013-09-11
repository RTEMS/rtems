/*
 *  lchown() - POSIX 1003.1b 5.6.5 - Change Owner and Group of a File
 *             But Do Not Follow a Symlink
 *
 *  Written by: Vinu Rajashekhar <vinutheraj@gmail.com>
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

int lchown( const char *path, uid_t owner, gid_t group )
{
  int rv;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_HARD_LINK;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  rv = rtems_filesystem_chown( currentloc, owner, group );

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
