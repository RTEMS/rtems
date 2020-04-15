/**
 *  @file
 *
 *  @brief Close Object
 *  @ingroup RTEMSScoreObject
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

void _Objects_Close(
  const Objects_Information *information,
  Objects_Control           *the_object
)
{
  _Objects_Invalidate_Id( information, the_object );

  _Objects_Namespace_remove_u32( information, the_object );
}
