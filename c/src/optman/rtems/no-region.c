/*
 *  Region Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/region.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/interr.h>

void _Region_Manager_initialization(
  unsigned32 maximum_regions
)
{
}

rtems_status_code rtems_region_create(
  rtems_name          name,
  void               *starting_address,
  unsigned32          length,
  unsigned32          page_size,
  rtems_attribute  attribute_set,
  Objects_Id         *id
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_region_ident(
  rtems_name    name,
  Objects_Id   *id
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_region_delete(
  Objects_Id id
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_region_get_segment(
  Objects_Id         id,
  unsigned32         size,
  rtems_option    option_set,
  rtems_interval  timeout,
  void              **segment
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_region_get_segment_size(
  Objects_Id         id,
  void              *segment,
  unsigned32        *size
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_region_return_segment(
  Objects_Id  id,
  void       *segment
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}
