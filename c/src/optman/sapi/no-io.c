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
#include <rtems/rtems/status.h>
#include <rtems/config.h>
#include <rtems/io.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>

void _IO_Initialize_all_drivers( void )
{
}

rtems_status_code rtems_io_register_name(
    char *device_name,
    rtems_device_major_number major,
    rtems_device_minor_number minor
  )
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_lookup_name(
    const char *pathname,
    rtems_driver_name_t **rnp
  )
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  return RTEMS_NOT_CONFIGURED;
}

rtems_status_code rtems_io_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument
)
{
  return RTEMS_NOT_CONFIGURED;
}
