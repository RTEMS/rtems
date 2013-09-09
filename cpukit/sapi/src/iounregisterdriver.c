/**
 * @file
 *
 * @brief Unregister a Driver from the Device Driver Table.
 *
 * @ingroup ClassicIO
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/io.h>
#include <rtems/rtems/intr.h>
#include <rtems/score/threaddispatch.h>

#include <string.h>

rtems_status_code rtems_io_unregister_driver(
  rtems_device_major_number major
)
{
  if ( rtems_interrupt_is_in_progress() )
    return RTEMS_CALLED_FROM_ISR;

  if ( major < _IO_Number_of_drivers ) {
    _Thread_Disable_dispatch();
    memset(
      &_IO_Driver_address_table[major],
      0,
      sizeof( rtems_driver_address_table )
    );
    _Thread_Enable_dispatch();

    return RTEMS_SUCCESSFUL;
  }

  return RTEMS_UNSATISFIED;
}
