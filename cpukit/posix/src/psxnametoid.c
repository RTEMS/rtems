/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/posix/posixapi.h>

#include <string.h>
#include <limits.h>
#include <errno.h>

/* pure ANSI mode does not have this prototype */
size_t strnlen(const char *, size_t);

int _POSIX_Name_to_id(
  Objects_Information *information,
  const char          *name,
  Objects_Id          *id,
  size_t              *len
)
{
  int eno = EINVAL;
  size_t n = 0;

  if ( name != NULL && name [0] != '\0' ) {
    n = strnlen( name, NAME_MAX );

    if ( n < NAME_MAX ) {
      Objects_Name_or_id_lookup_errors status = _Objects_Name_to_id_string(
        information,
        name,
        id
      );

      if ( status == OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL ) {
        eno = 0;
      } else {
        eno = ENOENT;
      }
    } else {
      eno = ENAMETOOLONG;
    }
  }

  *len = n;

  return eno;
}
