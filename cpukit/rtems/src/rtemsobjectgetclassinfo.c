/*
 *  RTEMS Object Helper -- Get Information on Class
 *
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
#include <rtems/rtems/status.h>
#include <rtems/score/object.h>
#include <rtems/rtems/object.h>

rtems_status_code rtems_object_get_class_information(
  int                                 the_api,
  int                                 the_class,
  rtems_object_api_class_information *info
)
{
  Objects_Information *obj_info;
  int                  unallocated;
  int                  i;

  /*
   * Validate parameters and look up information structure.
   */
  if ( !info )
    return RTEMS_INVALID_ADDRESS;

  obj_info = _Objects_Get_information( the_api, the_class );
  if ( !obj_info )
    return RTEMS_INVALID_NUMBER;

  /*
   * Return information about this object class to the user.
   */
  info->minimum_id  = obj_info->minimum_id;
  info->maximum_id  = obj_info->maximum_id;
  info->auto_extend = obj_info->auto_extend;
  info->maximum     = obj_info->maximum;

  for ( unallocated=0, i=1 ; i <= info->maximum ; i++ )
    if ( !obj_info->local_table[i] )
      unallocated++;

  info->unallocated = unallocated;

  return RTEMS_SUCCESSFUL;
}

