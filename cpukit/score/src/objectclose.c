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

void _Objects_Close(
  Objects_Information  *information,
  Objects_Control      *the_object
)
{
  _Objects_Invalidate_Id( information, the_object );

  _Objects_Namespace_remove( information, the_object );
}
