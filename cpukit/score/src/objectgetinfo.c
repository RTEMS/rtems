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
#include <rtems/score/wkspace.h>

Objects_Information *_Objects_Get_information(
  Objects_APIs   the_api,
  uint16_t       the_class
)
{
  Objects_Information *info;
  int the_class_api_maximum;

  if ( !the_class )
    return NULL;

  /*
   *  This call implicitly validates the_api so we do not call
   *  _Objects_Is_api_valid above here.
   */
  the_class_api_maximum = _Objects_API_maximum_class( the_api );
  if ( the_class_api_maximum == 0 )
    return NULL;

  if ( the_class > (uint32_t) the_class_api_maximum )
    return NULL;

  if ( !_Objects_Information_table[ the_api ] )
    return NULL;

  info = _Objects_Information_table[ the_api ][ the_class ];
  if ( !info )
    return NULL;

  /*
   *  In a multprocessing configuration, we may access remote objects.
   *  Thus we may have 0 local instances and still have a valid object
   *  pointer.
   */
  #if !defined(RTEMS_MULTIPROCESSING)
    if ( info->maximum == 0 )
      return NULL;
  #endif

  return info;
}

