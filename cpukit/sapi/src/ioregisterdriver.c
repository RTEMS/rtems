/**
 * @file
 *
 * @ingroup ClassicIO
 *
 * @brief Classic Input/Output Manager implementation.
 */
 
/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2009 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/io.h>
#include <rtems/rtems/intr.h>

rtems_status_code rtems_io_driver_io_error(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  return RTEMS_IO_ERROR;
}

static inline bool rtems_io_is_empty_table(
  const rtems_driver_address_table *table
)
{
  return table->initialization_entry == NULL && table->open_entry == NULL;
}

static inline void rtems_io_occupy_table(
  rtems_driver_address_table *table
)
{
  table->open_entry = rtems_io_driver_io_error;
}

static rtems_status_code rtems_io_obtain_major_number(
  rtems_device_major_number *major
)
{
  rtems_device_major_number n = _IO_Number_of_drivers;
  rtems_device_major_number m = 0;
  
  if ( major == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  for ( m = 0; m < n; ++m ) {
    rtems_driver_address_table *const table = _IO_Driver_address_table + m;
    rtems_interrupt_level level;

    rtems_interrupt_disable( level );
    if ( rtems_io_is_empty_table( table ) ) {
      rtems_io_occupy_table( table );
      rtems_interrupt_enable( level );

      break;
    }
    rtems_interrupt_enable( level );
  }

  /* Assigns invalid value in case of failure */
  *major = m;

  if ( m != n ) {
    return RTEMS_SUCCESSFUL;
  } else {
    return RTEMS_TOO_MANY;
  }
}

rtems_status_code rtems_io_register_driver(
  rtems_device_major_number major,
  const rtems_driver_address_table *driver_table,
  rtems_device_major_number *registered_major
)
{
  rtems_device_major_number major_limit = _IO_Number_of_drivers;

  if ( registered_major == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  /* Set it to an invalid value */
  *registered_major = major_limit;

  if ( driver_table == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( rtems_io_is_empty_table( driver_table ) ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( major >= major_limit ) {
    return RTEMS_INVALID_NUMBER;
  }

  if ( major == 0 ) {
    rtems_status_code sc = rtems_io_obtain_major_number( registered_major );

    if ( sc == RTEMS_SUCCESSFUL ) {
      major = *registered_major;
    } else {
      return RTEMS_TOO_MANY;
    }
  } else {
    rtems_driver_address_table *const table = _IO_Driver_address_table + major;
    rtems_interrupt_level level;

    rtems_interrupt_disable( level );
    if ( rtems_io_is_empty_table( table ) ) {
      rtems_io_occupy_table( table );
      rtems_interrupt_enable( level );
    } else {
      rtems_interrupt_enable( level );

      return RTEMS_RESOURCE_IN_USE;
    }

    *registered_major = major;
  }

  _IO_Driver_address_table [major] = *driver_table;

  return rtems_io_initialize( major, 0, NULL );
}
