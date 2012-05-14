/*
 *  RTEMS Object Helper -- Obtain Name of API
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
#include <rtems/rtems/object.h>

#include <rtems/assoc.h>

rtems_assoc_t rtems_objects_api_assoc[] = {
  { "Internal", OBJECTS_INTERNAL_API, 0},
  { "Classic",  OBJECTS_CLASSIC_API, 0},
  { "POSIX",    OBJECTS_POSIX_API, 0},
  { 0, 0, 0 }
};

const char *rtems_object_get_api_name(
  int api
)
{
  const rtems_assoc_t *api_assoc;

  api_assoc = rtems_assoc_ptr_by_local( rtems_objects_api_assoc, api );
  if ( api_assoc )
    return api_assoc->name;
  return "BAD CLASS";
}

