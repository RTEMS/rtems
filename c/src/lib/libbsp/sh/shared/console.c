/*
 * /dev/console for Hitachi SH 703X
 *
 * This driver installs an alternate device name (e.g. /dev/console for
 * the designated console device /dev/console.
 */

/*
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  COPYRIGHT (c) 1998, 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/console.h>

#include <sys/stat.h>

#ifndef BSP_CONSOLE_DEVNAME
#error Missing BSP_CONSOLE_DEVNAME
#endif

/*  console_initialize
 *
 *  This routine initializes the console IO driver.
 */
rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_device_driver status;
  struct stat         st;
  int                 rv;

  rv = stat( BSP_CONSOLE_DEVNAME, &st );
  if ( rv != 0 )
    rtems_fatal_error_occurred(rv);

  status = rtems_io_register_name(
    "/dev/console",
    rtems_filesystem_dev_major_t( st.st_rdev ),
    rtems_filesystem_dev_minor_t( st.st_rdev )
  );
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
  rtems_fatal_error_occurred(-1);
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
  rtems_fatal_error_occurred(-1);
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
  rtems_fatal_error_occurred(-1);
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
  rtems_fatal_error_occurred(-1);
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
  rtems_fatal_error_occurred(-1);
}
