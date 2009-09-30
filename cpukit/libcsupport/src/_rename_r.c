/*
 *  _rename_r() - POSIX 1003.1b - 5.3.4 - Rename a file
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE__RENAME_R)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <reent.h>

int _rename_r(
  struct _reent *ptr __attribute__((unused)),
  const char    *old,
  const char    *new
)
{
  struct stat sb;
  int s;

  s = stat( old, &sb);
  if ( s < 0 )
    return s;
  s = link( old, new );
  if ( s < 0 )
    return s;
  return S_ISDIR(sb.st_mode) ? rmdir( old ) : unlink( old );
}
#endif
