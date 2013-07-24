/**
 * @file
 *
 * @brief Get Lowest Valid Class Value
 * @ingroup ClassicClassInfo Object Class Information
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

#include <rtems/rtems/object.h>
#include <rtems/score/objectimpl.h>

int rtems_object_api_minimum_class(
  int api
)
{
  if ( _Objects_Is_api_valid( api ) )
    return 1;
  return -1;
}
