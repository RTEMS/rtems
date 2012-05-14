/*
 *  Barrier Manager -- Create a Barrier Instance
 *
 *  COPYRIGHT (c) 1989-2006.
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
#include <rtems/rtems/support.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/barrier.h>

/*
 *  rtems_barrier_create
 *
 *  This directive creates a barrier.  A barrier id is returned.
 *
 *  Input parameters:
 *    name             - user defined barrier name
 *    attribute_set    - barrier attributes
 *    maximum_waiters  - number of threads before automatic release
 *    priority_ceiling - barrier's ceiling priority
 *    id               - pointer to barrier id
 *
 *  Output parameters:
 *    id               - barrier id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_barrier_create(
  rtems_name           name,
  rtems_attribute      attribute_set,
  uint32_t             maximum_waiters,
  rtems_id            *id
)
{
  Barrier_Control         *the_barrier;
  CORE_barrier_Attributes  the_attributes;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  if ( !id )
    return RTEMS_INVALID_ADDRESS;

  /* Initialize core barrier attributes */
  if ( _Attributes_Is_barrier_automatic( attribute_set ) ) {
    the_attributes.discipline = CORE_BARRIER_AUTOMATIC_RELEASE;
    if ( maximum_waiters == 0 )
      return RTEMS_INVALID_NUMBER;
  } else
    the_attributes.discipline = CORE_BARRIER_MANUAL_RELEASE;
  the_attributes.maximum_count = maximum_waiters;

  _Thread_Disable_dispatch();             /* prevents deletion */

  the_barrier = _Barrier_Allocate();

  if ( !the_barrier ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

  the_barrier->attribute_set = attribute_set;

  _CORE_barrier_Initialize( &the_barrier->Barrier, &the_attributes );

  _Objects_Open(
    &_Barrier_Information,
    &the_barrier->Object,
    (Objects_Name) name
  );

  *id = the_barrier->Object.id;

  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}
