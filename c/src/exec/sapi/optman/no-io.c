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
#include <rtems/rtems/status.h>
#include <rtems/io.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/interr.h>

void _IO_Manager_initialization(
  rtems_driver_address_table *driver_table,
  unsigned32                  drivers_in_table,
  unsigned32                  number_of_drivers,
  unsigned32                  number_of_devices
)
{
}

void _IO_Initialize_all_drivers( void )
{
}

rtems_status_code rtems_io_register_driver(
    rtems_device_major_number   major,
    rtems_driver_address_table *driver_table,
    rtems_device_major_number  *registered_major
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}
  
rtems_status_code rtems_io_unregister_driver(
    rtems_device_major_number major
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}
  
rtems_status_code rtems_io_register_name(
    char *device_name,
    rtems_device_major_number major,
    rtems_device_minor_number minor
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_lookup_name(
    const char *pathname,
    rtems_driver_name_t **rnp
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    FALSE,
    RTEMS_NOT_CONFIGURED
  );
  return RTEMS_NOT_CONFIGURED;
}
