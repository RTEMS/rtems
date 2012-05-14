/*
 *  COPYRIGHT (c) 2009 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/*
 * The statvfs as defined by the SUS:
 *    http://www.opengroup.org/onlinepubs/009695399/basedefs/sys/statvfs.h.html
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <sys/statvfs.h>
#include <string.h>

#include <rtems/libio_.h>

int statvfs( const char *path, struct statvfs *buf )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_LINK;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  memset( buf, 0, sizeof( *buf ) );

  rv = (*currentloc->mt_entry->ops->statvfs_h)( currentloc, buf );

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
