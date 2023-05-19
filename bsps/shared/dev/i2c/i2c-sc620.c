/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief I2C Driver for SEMTECH SC620 Octal LED Driver
 */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
