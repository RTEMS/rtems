/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/devzero.h>

#include <string.h>

#include <rtems/libio.h>

rtems_device_driver dev_zero_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor __attribute__((unused)),
  void *arg __attribute__((unused))
)
{
  return rtems_io_register_name(DEVZERO_DEVICE_NAME, major, 0);
}

rtems_device_driver dev_zero_open(
  rtems_device_major_number major __attribute__((unused)),
  rtems_device_minor_number minor __attribute__((unused)),
  void *arg __attribute__((unused))
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dev_zero_close(
  rtems_device_major_number major __attribute__((unused)),
  rtems_device_minor_number minor __attribute__((unused)),
  void *arg __attribute__((unused))
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dev_zero_read(
  rtems_device_major_number major __attribute__((unused)),
  rtems_device_minor_number minor __attribute__((unused)),
  void *arg __attribute__((unused))
)
{
  rtems_libio_rw_args_t *rw = arg;

  rw->bytes_moved = rw->count;
  memset(rw->buffer, 0, rw->count);

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dev_zero_write(
  rtems_device_major_number major __attribute__((unused)),
  rtems_device_minor_number minor __attribute__((unused)),
  void *arg
)
{
  rtems_libio_rw_args_t *rw = arg;

  rw->bytes_moved = rw->count;

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver dev_zero_control(
  rtems_device_major_number major __attribute__((unused)),
  rtems_device_minor_number minor __attribute__((unused)),
  void *arg __attribute__((unused))
)
{
  return RTEMS_IO_ERROR;
}
