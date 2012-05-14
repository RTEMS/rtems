/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
  #if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
    /*
     *  If this is a string format name, then free the memory.
     */
    if ( information->is_string )
       _Workspace_Free( (void *)the_object->name.name_p );
  #endif

  /*
   * Clear out either format.
   */
  #if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
    the_object->name.name_p   = NULL;
  #endif
  the_object->name.name_u32 = 0;
}
