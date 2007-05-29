/*
 *  Input/Output Manager
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/io.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

#include <string.h>

/*PAGE
 *
 *  _IO_Manager_initialization
 *
 *  The IO manager has been extended to support runtime driver
 *  registration. The driver table is now allocated in the
 *  workspace.
 *
 */

void _IO_Manager_initialization(
  rtems_driver_address_table *driver_table,
  uint32_t                    drivers_in_table,
  uint32_t                    number_of_drivers
)
{
  uint32_t index;

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
  number_of_drivers = drivers_in_table;
}

/*PAGE
 *
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

/*PAGE
 *
 *  rtems_io_register_driver
 *
 *  Register a driver into the device driver table.
 *
 *  Input Paramters:
 *    major            - device major number (0 means allocate
 *                       a number)
 *    driver_table     - driver callout function table
 *    registered_major - the major number which is registered
 *
 *  Output Parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_io_register_driver(
  rtems_device_major_number   major,
  rtems_driver_address_table *driver_table,
  rtems_device_major_number  *registered_major
)
{

  /*
   *  Validate the pointer data and contents passed in
   */
  if ( !driver_table )
    return RTEMS_INVALID_ADDRESS;

  if ( !registered_major )
    return RTEMS_INVALID_ADDRESS;

  if ( !driver_table->initialization_entry && !driver_table->open_entry )
    return RTEMS_INVALID_ADDRESS;

  *registered_major = 0;

  /*
   *  The requested major number is higher than what is configured.
   */
  if ( major >= _IO_Number_of_drivers )
    return RTEMS_INVALID_NUMBER;

  /*
   * Test for initialise/open being present to indicate the driver slot is
   * in use.
   */

  if ( major == 0 ) {
    boolean found = FALSE;
    for ( major = _IO_Number_of_drivers - 1 ; major ; major-- ) {
      if ( !_IO_Driver_address_table[major].initialization_entry &&
           !_IO_Driver_address_table[major].open_entry ) {
        found = TRUE;
        break;
      }
    }

    if ( !found )
      return RTEMS_TOO_MANY;
  }

  if ( _IO_Driver_address_table[major].initialization_entry ||
       _IO_Driver_address_table[major].open_entry )
    return RTEMS_RESOURCE_IN_USE;


  _IO_Driver_address_table[major] = *driver_table;
  *registered_major               = major;

  rtems_io_initialize( major, 0, NULL );

  return RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  rtems_io_unregister_driver
 *
 *  Unregister a driver from the device driver table.
 *
 *  Input Paramters:
 *    major            - device major number
 *
 *  Output Parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_io_unregister_driver(
  rtems_device_major_number major
)
{
  if ( major < _IO_Number_of_drivers ) {
    memset(
      &_IO_Driver_address_table[major],
      0,
      sizeof( rtems_driver_address_table )
    );
    return RTEMS_SUCCESSFUL;
  }
  return RTEMS_UNSATISFIED;
}

/*PAGE
 *
 *  rtems_io_initialize
 *
 *  This routine is the initialization directive of the IO manager.
 *
 *  Input Paramters:
 *    major        - device driver number
 *    minor        - device number
 *    argument     - pointer to argument(s)
 *
 *  Output Parameters:
 *    returns       - return code
 */

rtems_status_code rtems_io_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
)
{
  rtems_device_driver_entry callout;

  if ( major >= _IO_Number_of_drivers )
    return RTEMS_INVALID_NUMBER;

  callout = _IO_Driver_address_table[major].initialization_entry;
  return callout ? callout(major, minor, argument) : RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  rtems_io_open
 *
 *  This routine is the open directive of the IO manager.
 *
 *  Input Paramters:
 *    major        - device driver number
 *    minor        - device number
 *    argument     - pointer to argument(s)
 *
 *  Output Parameters:
 *    returns       - return code
 */

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

/*PAGE
 *
 *  rtems_io_close
 *
 *  This routine is the close directive of the IO manager.
 *
 *  Input Paramters:
 *    major        - device driver number
 *    minor        - device number
 *    argument     - pointer to argument(s)
 *
 *  Output Parameters:
 *    returns       - return code
 */

rtems_status_code rtems_io_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
)
{
  rtems_device_driver_entry callout;

  if ( major >= _IO_Number_of_drivers )
    return RTEMS_INVALID_NUMBER;

  callout = _IO_Driver_address_table[major].close_entry;
  return callout ? callout(major, minor, argument) : RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  rtems_io_read
 *
 *  This routine is the read directive of the IO manager.
 *
 *  Input Paramters:
 *    major        - device driver number
 *    minor        - device number
 *    argument     - pointer to argument(s)
 *
 *  Output Parameters:
 *    returns       - return code
 */

rtems_status_code rtems_io_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
)
{
  rtems_device_driver_entry callout;

  if ( major >= _IO_Number_of_drivers )
    return RTEMS_INVALID_NUMBER;

  callout = _IO_Driver_address_table[major].read_entry;
  return callout ? callout(major, minor, argument) : RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  rtems_io_write
 *
 *  This routine is the write directive of the IO manager.
 *
 *  Input Paramters:
 *    major        - device driver number
 *    minor        - device number
 *    argument     - pointer to argument(s)
 *
 *  Output Parameters:
 *    returns       - return code
 */

rtems_status_code rtems_io_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
)
{
  rtems_device_driver_entry callout;

  if ( major >= _IO_Number_of_drivers )
    return RTEMS_INVALID_NUMBER;

  callout = _IO_Driver_address_table[major].write_entry;
  return callout ? callout(major, minor, argument) : RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  rtems_io_control
 *
 *  This routine is the control directive of the IO manager.
 *
 *  Input Paramters:
 *    major        - device driver number
 *    minor        - device number
 *    argument     - pointer to argument(s)
 *
 *  Output Parameters:
 *    returns       - return code
 */

rtems_status_code rtems_io_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
)
{
  rtems_device_driver_entry callout;

  if ( major >= _IO_Number_of_drivers )
    return RTEMS_INVALID_NUMBER;

  callout = _IO_Driver_address_table[major].control_entry;
  return callout ? callout(major, minor, argument) : RTEMS_SUCCESSFUL;
}
