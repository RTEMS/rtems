/**
 * @file
 *
 * @brief Opening for The IO Manager
 *
 * @ingroup ClassicIO
 */

/*
 *  Input/Output Manager - Open Device
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/ioimpl.h>

rtems_status_code rtems_io_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
)
{
  rtems_device_driver_entry callout;

  if ( major >= _IO_Number_of_drivers )
    return RTEMS_INVALID_NUMBER;

  callout = _IO_Driver_address_table[major].open_entry;
  return callout ? callout(major, minor, argument) : RTEMS_SUCCESSFUL;
}
