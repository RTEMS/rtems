/*
 *  RTEMS Object Helper -- Build an Object Id
 *
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
#include <rtems/rtems/types.h>

#undef rtems_build_id
rtems_id rtems_build_id(
  uint32_t api,
  uint32_t class,
  uint32_t node,
  uint32_t index
)
{
  return _Objects_Build_id( api, class, node, index );
}
