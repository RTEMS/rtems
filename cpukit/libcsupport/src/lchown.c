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
  return rtems_filesystem_chown(
    path,
    owner,
    group,
    RTEMS_FS_FOLLOW_HARD_LINK
  );
}
