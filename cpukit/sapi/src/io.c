/*
 *  Input/Output Manager - Initialize Device Driver Subsystem
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
#include <rtems/config.h>
#include <rtems/io.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

#include <string.h>

/*
 *  _IO_Manager_initialization
 *
 *  The IO manager has been extended to support runtime driver
 *  registration. The driver table is now allocated in the
 *  workspace.
 *
 */

void _IO_Manager_initialization(void)
{
  uint32_t                    index;
  rtems_driver_address_table *driver_table;
  uint32_t                    drivers_in_table;
  uint32_t                    number_of_drivers;

  driver_table      = Configuration.Device_driver_table;
  drivers_in_table  = Configuration.number_of_device_drivers;
  number_of_drivers = Configuration.maximum_drivers;

  /*
   *  If the user claims there are less drivers than are actually in
   *  the table, then let's just go with the table's count.
   */
  if ( number_of_drivers <= drivers_in_table )
    number_of_drivers = drivers_in_table;

  /*
   *  If the maximum number of driver is the same as the number in the
   *  table, then we do not have to copy the driver table.  They can't
   *  register any dynamically.
   */
  if ( number_of_drivers == drivers_in_table ) {
    _IO_Driver_address_table = driver_table;
    _IO_Number_of_drivers = number_of_drivers;
    return;
  }

  /*
   *  The application requested extra slots in the driver table, so we
   *  have to allocate a new driver table and copy theirs to it.
   */

  _IO_Driver_address_table = (rtems_driver_address_table *)
      _Workspace_Allocate_or_fatal_error(
        sizeof( rtems_driver_address_table ) * ( number_of_drivers )
      );
  _IO_Number_of_drivers = number_of_drivers;

  memset(
    _IO_Driver_address_table, 0,
    sizeof( rtems_driver_address_table ) * ( number_of_drivers )
  );

  for ( index = 0 ; index < drivers_in_table ; index++ )
    _IO_Driver_address_table[index] = driver_table[index];
}

/*
 *  _IO_Initialize_all_drivers
 *
 *  This routine initializes all device drivers
 *
 *  Input Paramters:   NONE
 *
 *  Output Parameters: NONE
 */

void _IO_Initialize_all_drivers( void )
{
   rtems_device_major_number major;

   for ( major=0 ; major < _IO_Number_of_drivers ; major ++ )
     (void) rtems_io_initialize( major, 0, NULL );
}
