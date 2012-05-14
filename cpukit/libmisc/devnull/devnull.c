/*  /dev/null
 *
 *  Derived from rtems' stub driver.
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/devnull.h>
#include <rtems/libio.h>

/*  null_initialize
 *
 *  This routine is the null device driver init routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargp - pointer to parameter block
 *
 *  Output parameters:
 *    rval       - NULL_SUCCESSFUL
 */

uint32_t   NULL_major;
static char initialized;

rtems_device_driver null_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor __attribute__((unused)),
  void *pargp __attribute__((unused))
)
{
  rtems_device_driver status;

  if ( !initialized ) {
    initialized = 1;

    status = rtems_io_register_name(
      "/dev/null",
      major,
      (rtems_device_minor_number) 0
    );

    if (status != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred(status);

    NULL_major = major;
  }

  return RTEMS_SUCCESSFUL;
}

/*  null_open
 *
 *  This routine is the null device driver open routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargb - pointer to open parameter block
 *
 *  Output parameters:
 *    rval       - NULL_SUCCESSFUL
 */

rtems_device_driver null_open(
  rtems_device_major_number major __attribute__((unused)),
  rtems_device_minor_number minor __attribute__((unused)),
  void *pargp __attribute__((unused))
)
{
  return NULL_SUCCESSFUL;
}


/*  null_close
 *
 *  This routine is the null device driver close routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargb - pointer to close parameter block
 *
 *  Output parameters:
 *    rval       - NULL_SUCCESSFUL
 */

rtems_device_driver null_close(
  rtems_device_major_number major __attribute__((unused)),
  rtems_device_minor_number minor __attribute__((unused)),
  void *pargp __attribute__((unused))
)
{
  return NULL_SUCCESSFUL;
}


/*  null_read
 *
 *  This routine is the null device driver read routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargp - pointer to read parameter block
 *
 *  Output parameters:
 *    rval       - NULL_SUCCESSFUL
 */

rtems_device_driver null_read(
  rtems_device_major_number major __attribute__((unused)),
  rtems_device_minor_number minor __attribute__((unused)),
  void *pargp __attribute__((unused))
)
{
  return NULL_SUCCESSFUL;
}


/*  null_write
 *
 *  This routine is the null device driver write routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargp - pointer to write parameter block
 *
 *  Output parameters:
 *    rval       - NULL_SUCCESSFUL
 */

rtems_device_driver null_write(
  rtems_device_major_number major __attribute__((unused)),
  rtems_device_minor_number minor __attribute__((unused)),
  void *pargp
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) pargp;

  if ( rw_args )
    rw_args->bytes_moved = rw_args->count;

  return NULL_SUCCESSFUL;
}


/*  null_control
 *
 *  This routine is the null device driver control routine.
 *
 *  Input parameters:
 *    major - device major number
 *    minor - device minor number
 *    pargp - pointer to cntrl parameter block
 *
 *  Output parameters:
 *    rval       - NULL_SUCCESSFUL
 */

rtems_device_driver null_control(
  rtems_device_major_number major __attribute__((unused)),
  rtems_device_minor_number minor __attribute__((unused)),
  void *pargp __attribute__((unused))
)
{
  return NULL_SUCCESSFUL;
}
