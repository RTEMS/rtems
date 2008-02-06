/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/wkspace.h>

void _Objects_Namespace_remove(
  Objects_Information  *information,
  Objects_Control      *the_object
)
{
  /*
   *  If this is a string format name, then free the memory.
   */
  if ( information->is_string && the_object->name.name_p )
     _Workspace_Free( (void *)the_object->name.name_p );

  /*
   * Clear out either format.
   */
  the_object->name.name_p   = NULL;
  the_object->name.name_u32 = 0;
}
