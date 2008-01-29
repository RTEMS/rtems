/*
 *  RTEMS Object Helper -- Get Least Valid Class for an API
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/rtems/types.h>

uint32_t rtems_object_api_minimum_class(
  uint32_t api
)
{
  if ( _Objects_Is_api_valid( api ) )
    return 1;
  return -1;
}
