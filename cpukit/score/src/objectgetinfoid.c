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

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>

Objects_Information *_Objects_Get_information_id(
  Objects_Id  id
)
{
  return _Objects_Get_information(
    _Objects_Get_API( id ),
    _Objects_Get_class( id )
  );
}
