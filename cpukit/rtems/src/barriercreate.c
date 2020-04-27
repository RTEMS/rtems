/**
 * @file
 *
 * @ingroup ClassicBarrier Barriers
 *
 * @brief RTEMS Create Barrier
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/barrierimpl.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/score/isr.h>
#include <rtems/sysinit.h>

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

  the_barrier = _Barrier_Allocate();

  if ( !the_barrier ) {
    _Objects_Allocator_unlock();
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

  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}

static void _Barrier_Manager_initialization( void )
{
  _Objects_Initialize_information( &_Barrier_Information );
}

RTEMS_SYSINIT_ITEM(
  _Barrier_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_BARRIER,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
