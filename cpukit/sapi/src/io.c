/*
 *  Input/Output Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

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
    unsigned32                  drivers_in_table,
    unsigned32                  number_of_drivers,
    unsigned32                  number_of_devices
)
{
  void                *tmp;
  unsigned32           index;
  rtems_driver_name_t *np;

  if ( number_of_drivers < drivers_in_table )
      number_of_drivers = drivers_in_table;
  
  tmp = _Workspace_Allocate_or_fatal_error(
    sizeof( rtems_driver_address_table ) * ( number_of_drivers )
  );
 
  _IO_Driver_address_table = (rtems_driver_address_table *) tmp;

  memset(
    _IO_Driver_address_table, 0,
    sizeof( rtems_driver_address_table ) * ( number_of_drivers )
  );

  if ( drivers_in_table )
      for ( index = 0 ; index < drivers_in_table ; index++ )
        _IO_Driver_address_table[index] = driver_table[index];
  
  _IO_Number_of_drivers = number_of_drivers;
  _IO_Number_of_devices = number_of_devices;
  
  tmp = _Workspace_Allocate_or_fatal_error(
    sizeof( rtems_driver_name_t ) * ( number_of_devices + 1 )
  );
 
  _IO_Driver_name_table = (rtems_driver_name_t *) tmp;
 
  for( index=0, np = _IO_Driver_name_table ;
       index < _IO_Number_of_devices ;
       index++, np++ ) {
    np->device_name = 0;
    np->device_name_length = 0;
    np->major = 0;
    np->minor = 0;
  }
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
     (void) rtems_io_initialize( major, 0, NULL);
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
    *registered_major = 0;

    /*
     * Test for initialise/open being present to indicate the driver slot is
     * in use.
     */

    if ( major >= _IO_Number_of_drivers )
      return RTEMS_INVALID_NUMBER;

    if ( major == 0 )
    {
        for ( major = _IO_Number_of_drivers - 1 ; major ; major-- )
            if ( _IO_Driver_address_table[major].initialization_entry == 0 &&
                 _IO_Driver_address_table[major].open_entry == 0 )
                break;

        if (( major == 0 ) &&
            ( _IO_Driver_address_table[major].initialization_entry == 0 &&
              _IO_Driver_address_table[major].open_entry == 0 ))
            return RTEMS_TOO_MANY;
    }
    
    if ( _IO_Driver_address_table[major].initialization_entry == 0 &&
         _IO_Driver_address_table[major].open_entry == 0 )
    {
        _IO_Driver_address_table[major] = *driver_table;
        *registered_major               = major;

        rtems_io_initialize( major, 0, NULL );

        return RTEMS_SUCCESSFUL;
    }

    return RTEMS_RESOURCE_IN_USE;
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
    if ( major < _IO_Number_of_drivers )
    {
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
 *  rtems_io_register_name
 *
 *  Associate a name with a driver
 *
 *  Input Paramters:
 *    device_name - pointer to name string to associate with device
 *    major       - device major number to receive name
 *    minor       - device minor number to receive name
 *
 *  Output Parameters: 
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

#if 0
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
#endif

/*PAGE
 *
 *  rtems_io_lookup_name
 *
 *  Find what driver "owns" this name
 *
 *  Input Paramters:
 *    name - name to lookup the associated device
 *
 *  Output Parameters: 
 *    device_info      - device associate with name
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

#if 0
rtems_status_code rtems_io_lookup_name(
    const char           *name,
    rtems_driver_name_t **device_info
)
{
    rtems_driver_name_t *np;
    unsigned32 index;

    for( index=0, np = _IO_Driver_name_table ;
         index < _IO_Number_of_devices ; 
         index++, np++ )
        if (np->device_name)
            if (strncmp(np->device_name, name, np->device_name_length) == 0)
            {                
                *device_info = np;
                return RTEMS_SUCCESSFUL;
            }
    
    *device_info = 0;
    return RTEMS_UNSATISFIED;
}
#endif


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

