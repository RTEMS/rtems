/*
 *  Region Manager
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/object.h>
#include <rtems/options.h>
#include <rtems/region.h>
#include <rtems/states.h>
#include <rtems/thread.h>

void _Region_Manager_initialization(
  unsigned32 maximum_regions
)
{
}

rtems_status_code rtems_region_create(
  Objects_Name        name,
  void               *starting_address,
  unsigned32          length,
  unsigned32          page_size,
  rtems_attribute  attribute_set,
  Objects_Id         *id
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_region_ident(
  Objects_Name  name,
  Objects_Id   *id
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_region_delete(
  Objects_Id id
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_region_get_segment(
  Objects_Id         id,
  unsigned32         size,
  rtems_option    option_set,
  rtems_interval  timeout,
  void              **segment
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_region_get_segment_size(
  Objects_Id         id,
  void              *segment,
  unsigned32        *size
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_region_return_segment(
  Objects_Id  id,
  void       *segment
)
{
  return( RTEMS_NOT_CONFIGURED );
}
