/**
 * @file
 *
 * @ingroup RTEMSImplClassicBarrier
 *
 * @brief This source file contains the implementation of
 *   rtems_barrier_create() and the Barrier Manager system initialization.
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
  Barrier_Control *the_barrier;
  uint32_t         maximum_count;

  if ( !rtems_is_name_valid( name ) ) {
    return RTEMS_INVALID_NAME;
  }

  if ( id == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( _Attributes_Is_barrier_automatic( attribute_set ) ) {
    if ( maximum_waiters == 0 ) {
      return RTEMS_INVALID_NUMBER;
    }

    maximum_count = maximum_waiters;
  } else {
    maximum_count = CORE_BARRIER_MANUAL_RELEASE_MAXIMUM_COUNT;
  }

  the_barrier = _Barrier_Allocate();

  if ( the_barrier == NULL ) {
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

  _CORE_barrier_Initialize( &the_barrier->Barrier, maximum_count );

  *id = _Objects_Open_u32( &_Barrier_Information, &the_barrier->Object, name );
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
