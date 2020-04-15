/**
 * @file
 *
 * @brief Removes Object from Namespace
 *
 * @ingroup RTEMSScore
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>
#include <rtems/score/wkspace.h>

void _Objects_Namespace_remove_string(
  const Objects_Information *information,
  Objects_Control           *the_object
)
{
  char *name;

  _Assert( _Objects_Has_string_name( information ) );
  name = RTEMS_DECONST( char *, the_object->name.name_p );
  the_object->name.name_p = NULL;
  _Workspace_Free( name );
}
