/*
 *  Input/Output Manager
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
#include <rtems/io.h>
#include <rtems/isr.h>
#include <rtems/thread.h>

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
   unsigned32                ignored;

   for ( major=0 ; major < _IO_Number_of_drivers ; major ++ )
     (void) rtems_io_initialize( major, 0, _Configuration_Table, &ignored );
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
 *    return_value - pointer to driver's return value
 *
 *  Output Parameters:
 *    returns       - return code
 *    *return_value - driver's return code
 */

rtems_status_code rtems_io_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
)
{
  return _IO_Handler_routine(
            IO_INITIALIZE_OPERATION,
            major,
            minor,
            argument,
            return_value
         );
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
 *    return_value - pointer to driver's return value
 *
 *  Output Parameters:
 *    returns       - return code
 *    *return_value - driver's return code
 */

rtems_status_code rtems_io_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
)
{
  return _IO_Handler_routine(
            IO_OPEN_OPERATION,
            major,
            minor,
            argument,
            return_value
         );
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
 *    return_value - pointer to driver's return value
 *
 *  Output Parameters:
 *    returns       - return code
 *    *return_value - driver's return code
 */

rtems_status_code rtems_io_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
)
{
  return _IO_Handler_routine(
            IO_CLOSE_OPERATION,
            major,
            minor,
            argument,
            return_value
         );
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
 *    return_value - pointer to driver's return value
 *
 *  Output Parameters:
 *    returns       - return code
 *    *return_value - driver's return code
 */

rtems_status_code rtems_io_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
)
{
  return _IO_Handler_routine(
            IO_READ_OPERATION,
            major,
            minor,
            argument,
            return_value
         );
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
 *    return_value - pointer to driver's return value
 *
 *  Output Parameters:
 *    returns       - return code
 *    *return_value - driver's return code
 */

rtems_status_code rtems_io_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
)
{
  return _IO_Handler_routine(
            IO_WRITE_OPERATION,
            major,
            minor,
            argument,
            return_value
         );
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
 *    return_value - pointer to driver's return value
 *
 *  Output Parameters:
 *    returns       - return code
 *    *return_value - driver's return code
 */

rtems_status_code rtems_io_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
)
{
  return _IO_Handler_routine(
            IO_CONTROL_OPERATION,
            major,
            minor,
            argument,
            return_value
         );
}

/*PAGE
 *
 *  _IO_Handler_routine
 *
 *  This routine implements all IO manager directives.
 *
 *  Input Paramters:
 *    operation    - I/O operation to be performed
 *    major        - device driver number
 *    minor        - device number
 *    argument     - pointer to argument(s)
 *    return_value - pointer to driver's return value
 *
 *  Output Parameters:
 *    returns       - return code
 *    *return_value - driver's return code
 */

rtems_status_code _IO_Handler_routine(
  IO_operations              operation,
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument,
  unsigned32                *return_value
)
{
  rtems_device_driver_entry io_callout;

  /*
   *  NOTE:  There is no range checking as in Ada because:
   *           + arrays in Ada are not always zero based.
   *           + with zero based arrays, a comparison of an unsigned
   *             number being less than zero would be necessary to
   *             check it as a range.  This would cause a warning for
   *             checking an unsigned number for being negative.
   */

  if ( major >= _IO_Number_of_drivers )
    return ( RTEMS_INVALID_NUMBER );

  switch ( operation ) {
     case IO_INITIALIZE_OPERATION:
        io_callout = _IO_Driver_address_table[ major ].initialization;
        break;
     case IO_OPEN_OPERATION:
        io_callout = _IO_Driver_address_table[ major ].open;
        break;
     case IO_CLOSE_OPERATION:
        io_callout = _IO_Driver_address_table[ major ].close;
        break;
     case IO_READ_OPERATION:
        io_callout = _IO_Driver_address_table[ major ].read;
        break;
     case IO_WRITE_OPERATION:
        io_callout = _IO_Driver_address_table[ major ].write;
        break;
     case IO_CONTROL_OPERATION:
        io_callout = _IO_Driver_address_table[ major ].control;
        break;
     default:             /* unreached -- only to remove warnings */
        io_callout = NULL;
        break;
  }

  if ( io_callout != NULL )
     (*io_callout)(
        major,
        minor,
        argument,
        _Thread_Executing->Object.id,
        return_value
     );
  else
     *return_value = 0;

  return( RTEMS_SUCCESSFUL );
}
