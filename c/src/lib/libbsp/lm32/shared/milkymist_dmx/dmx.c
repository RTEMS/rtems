/*  dmx.c
 *
 *  Milkymist DMX512 driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011 Sebastien Bourdeauducq
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <stdlib.h>
#include <sys/types.h>
#include <rtems.h>
#include <rtems/status-checks.h>
#include <bsp.h>
#include <rtems/libio.h>
#include "../include/system_conf.h"
#include "milkymist_dmx.h"

#define IN_DEVICE_NAME "/dev/dmx_in"
#define OUT_DEVICE_NAME "/dev/dmx_out"

rtems_device_driver dmx_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;

  sc = rtems_io_register_name(IN_DEVICE_NAME, major, 0);
  RTEMS_CHECK_SC(sc, "create DMX input device");

  sc = rtems_io_register_name(OUT_DEVICE_NAME, major, 1);
  RTEMS_CHECK_SC(sc, "create DMX output device");

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dmx_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  int len;
  unsigned int i;
  unsigned char *values = (unsigned char *)rw_args->buffer;

  len = 512 - rw_args->offset;
  if (len < 0) {
    rw_args->bytes_moved = 0;
    return RTEMS_SUCCESSFUL;
  }
  if (len > rw_args->count)
    len = rw_args->count;

  if (minor == 0) {
    for (i=0;i<len;i++)
      values[i] = MM_READ(MM_DMX_RX((unsigned int)rw_args->offset+i));
  } else {
    for (i=0;i<len;i++)
      values[i] = MM_READ(MM_DMX_TX((unsigned int)rw_args->offset+i));
  }

  rw_args->bytes_moved = len;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dmx_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;
  int len;
  unsigned int i;
  unsigned char *values = (unsigned char *)rw_args->buffer;

  if (minor == 0) {
    rw_args->bytes_moved = 0;
    return RTEMS_UNSATISFIED;
  }

  len = 512 - rw_args->offset;
  if (len < 0) {
    rw_args->bytes_moved = 0;
    return RTEMS_SUCCESSFUL;
  }
  if (len > rw_args->count)
    len = rw_args->count;

  for (i=0;i<len;i++)
    MM_WRITE(MM_DMX_TX((unsigned int)rw_args->offset+i), values[i]);

  rw_args->bytes_moved = len;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dmx_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_libio_ioctl_args_t *args = arg;

  switch (args->command) {
    case DMX_SET_THRU:
      args->ioctl_return = 0;
      MM_WRITE(MM_DMX_THRU, (unsigned int)args->buffer);
      return RTEMS_SUCCESSFUL;
    case DMX_GET_THRU:
      args->ioctl_return = 0;
      *((unsigned int *)args->buffer) = MM_READ(MM_DMX_THRU);
      return RTEMS_SUCCESSFUL;
    default:
      args->ioctl_return = -1;
      return RTEMS_UNSATISFIED;
  }
}
