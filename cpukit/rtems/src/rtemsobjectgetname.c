/**
 *  @file
 *
 *  @brief Obtain Object Name as String
 *  @ingroup ClassicClassInfo
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>

/* XXX: This is for language bindings */
char *rtems_object_get_name(
  Objects_Id     id,
  size_t         length,
  char          *name
);

char *rtems_object_get_name(
  Objects_Id     id,
  size_t         length,
  char          *name
)
{
  return _Objects_Get_name_as_string( id, length, name );
}
