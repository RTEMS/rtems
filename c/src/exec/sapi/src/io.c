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
#include <rtems/io.h>
#include <rtems/core/isr.h>
#include <rtems/core/thread.h>

#include <string.h>

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
     (void) rtems_io_initialize( major, 0, NULL);
}

/*PAGE
 *
 *  rtems_io_register_name
 *
 *  Associate a name with a driver
 *
 *  Input Paramters:
 *
 *  Output Parameters: 
 */

rtems_status_code rtems_io_register_name(
    char *device_name,
    rtems_device_major_number major,
    rtems_device_minor_number minor
  )
{
    rtems_driver_name_t *np;
    unsigned32 level;
    unsigned32 index;

    /* find an empty slot */
    for( index=0, np = _IO_Driver_name_table ;
         index < _IO_Number_of_devices ; 
         index++, np++ )
    {

        _ISR_Disable(level);
        if (np->device_name == 0)
        {
            np->device_name = device_name;
            np->device_name_length = strlen(device_name);
            np->major = major;
            np->minor = minor;
            _ISR_Enable(level);

            return RTEMS_SUCCESSFUL;
        }
        _ISR_Enable(level);
    }

    return RTEMS_TOO_MANY;
}

/*PAGE
 *
 *  rtems_io_lookup_name
 *
 *  Find what driver "owns" this name
 *
 *  Input Paramters:
 *
 *  Output Parameters: 
 */

rtems_status_code rtems_io_lookup_name(
    const char           *pathname,
    rtems_driver_name_t **rnp
)
{
    rtems_driver_name_t *np;
    unsigned32 index;

    for( index=0, np = _IO_Driver_name_table ;
         index < _IO_Number_of_devices ; 
         index++, np++ )
        if (np->device_name)
            if (strncmp(np->device_name, pathname, np->device_name_length) == 0)
            {                
                *rnp = np;
                return RTEMS_SUCCESSFUL;
            }
    
    *rnp = 0;
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
  void             *argument
)
{
    rtems_device_driver_entry callout;
    
    if ( major >= _IO_Number_of_drivers )
        return RTEMS_INVALID_NUMBER;

    callout = _IO_Driver_address_table[major].initialization;
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
  void             *argument
)
{
    rtems_device_driver_entry callout;
    
    if ( major >= _IO_Number_of_drivers )
        return RTEMS_INVALID_NUMBER;

    callout = _IO_Driver_address_table[major].open;
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
  void             *argument
)
{
    rtems_device_driver_entry callout;
    
    if ( major >= _IO_Number_of_drivers )
        return RTEMS_INVALID_NUMBER;

    callout = _IO_Driver_address_table[major].close;
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
  void             *argument
)
{
    rtems_device_driver_entry callout;
    
    if ( major >= _IO_Number_of_drivers )
        return RTEMS_INVALID_NUMBER;

    callout = _IO_Driver_address_table[major].read;
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
  void             *argument
)
{
    rtems_device_driver_entry callout;
    
    if ( major >= _IO_Number_of_drivers )
        return RTEMS_INVALID_NUMBER;

    callout = _IO_Driver_address_table[major].write;
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
  void             *argument
)
{
    rtems_device_driver_entry callout;
    
    if ( major >= _IO_Number_of_drivers )
        return RTEMS_INVALID_NUMBER;

    callout = _IO_Driver_address_table[major].control;
    return callout ? callout(major, minor, argument) : RTEMS_SUCCESSFUL;
}

