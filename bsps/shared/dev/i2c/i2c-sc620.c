/**
 * @file
 *
 * @brief I2C Driver for SEMTECH SC620 Octal LED Driver
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <libchip/i2c-sc620.h>

#include <rtems/libio.h>

#define SC620_REG_COUNT 10

static rtems_status_code i2c_sc620_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc = RTEMS_IO_ERROR;
  rtems_libio_rw_args_t *rw = arg;
  unsigned char *buf = (unsigned char *) &rw->buffer[0];

  if (rw->count == 2 && buf[0] < SC620_REG_COUNT) {
    int rv;

    rv = rtems_libi2c_start_write_bytes(
      minor, buf, 2
    );
    if (rv == 2) {
      sc = rtems_libi2c_send_stop(minor);
    }
  }

  rw->bytes_moved = sc == RTEMS_SUCCESSFUL ? 2 : 0;

  return sc;
}

static rtems_status_code i2c_sc620_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc = RTEMS_IO_ERROR;
  rtems_libio_rw_args_t *rw = arg;
  unsigned char *buf = (unsigned char *) &rw->buffer[0];

  if (rw->count == 1 && buf[0] < SC620_REG_COUNT) {
    int rv;

    rv = rtems_libi2c_start_write_bytes(minor, buf, 1);
    if (rv == 1) {
      sc = rtems_libi2c_send_addr(minor, 0);
      if (sc == RTEMS_SUCCESSFUL) {
        rv = rtems_libi2c_read_bytes(minor, buf, 1);
        if (rv == 1) {
          sc = rtems_libi2c_send_stop(minor);
        }
      }
    }
  }

  rw->bytes_moved = sc == RTEMS_SUCCESSFUL ? 1 : 0;

  return sc;
}

static rtems_driver_address_table i2c_sc620_ops = {
  .read_entry =  i2c_sc620_read,
  .write_entry = i2c_sc620_write
};

rtems_libi2c_drv_t i2c_sc620_driver = {
  .ops = &i2c_sc620_ops,
  .size = sizeof(i2c_sc620_driver)
};
