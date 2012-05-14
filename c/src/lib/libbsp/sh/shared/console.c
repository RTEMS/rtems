/*
 * /dev/console for Hitachi SH 703X
 *
 * The SH doesn't have a designated console device. Therefore we "alias"
 * another device as /dev/console and revector all calls to /dev/console
 * to this device.
 *
 * This approach is similar to installing a sym-link from one device to
 * another device. If rtems once will support sym-links for devices files,
 * this implementation could be dropped.
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/iosupp.h>

#ifndef BSP_CONSOLE_DEVNAME
#error Missing BSP_CONSOLE_DEVNAME
#endif

static rtems_driver_name_t low_level_device_info;

/*  console_initialize
 *
 *  This routine initializes the console IO driver.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_device_driver status;

  status = rtems_io_register_name(
    "/dev/console",
    major,
    (rtems_device_minor_number) 0
  );

  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  status = rtems_io_lookup_name( BSP_CONSOLE_DEVNAME, &low_level_device_info );
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  return RTEMS_SUCCESSFUL;
}

/*
 *  Open entry point
 */

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_io_open( low_level_device_info.major,
    low_level_device_info.minor,
    arg );
}

/*
 *  Close entry point
 */

rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_io_close( low_level_device_info.major,
    low_level_device_info.minor,
    arg );
}

/*
 * read bytes from the serial port. We only have stdin.
 */

rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_io_read( low_level_device_info.major,
    low_level_device_info.minor,
    arg );
}

/*
 * write bytes to the serial port. Stdout and stderr are the same.
 */

rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_io_write( low_level_device_info.major,
    low_level_device_info.minor,
    arg );
}

/*
 *  IO Control entry point
 */

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_io_control( low_level_device_info.major,
    low_level_device_info.minor,
    arg );
}
