/*
 *  link() - POSIX 1003.1b - 5.3.4 - Create a new link
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

#include <unistd.h>

#include <rtems/libio_.h>

int link( const char *path1, const char *path2 )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx_1;
  rtems_filesystem_eval_path_context_t ctx_2;
  int eval_flags_1 = RTEMS_FS_FOLLOW_LINK;
  int eval_flags_2 = RTEMS_FS_FOLLOW_LINK
    | RTEMS_FS_MAKE
    | RTEMS_FS_EXCLUSIVE;
  const rtems_filesystem_location_info_t *currentloc_1 =
    rtems_filesystem_eval_path_start( &ctx_1, path1, eval_flags_1 );
  const rtems_filesystem_location_info_t *currentloc_2 =
    rtems_filesystem_eval_path_start( &ctx_2, path2, eval_flags_2 );

  rv = rtems_filesystem_location_exists_in_same_fs_instance_as(
    currentloc_1,
    currentloc_2
  );
  if ( rv == 0 ) {
    rv = (*currentloc_2->mt_entry->ops->link_h)(
      currentloc_2,
      currentloc_1,
      rtems_filesystem_eval_path_get_token( &ctx_2 ),
      rtems_filesystem_eval_path_get_tokenlen( &ctx_2 )
    );
  }

  rtems_filesystem_eval_path_cleanup( &ctx_1 );
  rtems_filesystem_eval_path_cleanup( &ctx_2 );

  return rv;
}

/*
 *  _link_r
 *
 *  This is the Newlib dependent reentrant version of link().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

int _link_r(
  struct _reent *ptr __attribute__((unused)),
  const char    *path1,
  const char    *path2
)
{
  return link( path1, path2 );
}
#endif
