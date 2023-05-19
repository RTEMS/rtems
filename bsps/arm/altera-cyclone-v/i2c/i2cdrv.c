/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycVI2C
 */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

#include <bsp.h>
#include <bsp/i2cdrv.h>
#include <assert.h>
#include <rtems/libio.h>
#include "i2cdrv-config.h"

typedef struct {
  ALT_I2C_DEV_t i2c_dev;
  rtems_id mutex;
} i2cdrv_entry;

i2cdrv_entry i2cdrv_table[CYCLONE_V_NO_I2C];

static ALT_I2C_DEV_t *get_device(i2cdrv_entry *e)
{
  return &e->i2c_dev;
}

static rtems_status_code init_i2c_module(
  i2cdrv_entry *e,
  const i2cdrv_configuration *cfg
)
{
  ALT_STATUS_CODE asc = ALT_E_SUCCESS;
  ALT_I2C_CTLR_t controller = cfg->controller;
  ALT_I2C_DEV_t *dev = get_device(e);
  ALT_I2C_MASTER_CONFIG_t i2c_cfg = {
    .addr_mode = ALT_I2C_ADDR_MODE_7_BIT,
    .restart_enable = false,
  };

  asc = alt_i2c_init(controller, dev);
  if ( asc != ALT_E_SUCCESS ) {
    return RTEMS_IO_ERROR;
  }
  asc = alt_i2c_op_mode_set(dev, ALT_I2C_MODE_MASTER);
  if ( asc != ALT_E_SUCCESS ) {
    return RTEMS_IO_ERROR;
  }
  asc = alt_i2c_master_config_speed_set(dev, &i2c_cfg, cfg->speed);
  if ( asc != ALT_E_SUCCESS ) {
    return RTEMS_IO_ERROR;
  }
  asc = alt_i2c_master_config_set(dev, &i2c_cfg);
  if ( asc != ALT_E_SUCCESS ) {
    return RTEMS_IO_ERROR;
  }
  asc = alt_i2c_enable(dev);
  if ( asc != ALT_E_SUCCESS ) {
    return RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver i2cdrv_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  for ( size_t i = 0; i < CYCLONE_V_NO_I2C; ++i ) {
    i2cdrv_entry *e = &i2cdrv_table[i];
    const i2cdrv_configuration *cfg = &i2cdrv_config[i];

    sc = rtems_io_register_name(cfg->device_name, major, i);
    assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_semaphore_create(
      rtems_build_name ('I', '2', 'C', '0' + i),
      0,
      RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
      0,
      &e->mutex
    );
    assert(sc == RTEMS_SUCCESSFUL);

    sc = init_i2c_module(e, cfg);
    if ( sc != RTEMS_SUCCESSFUL ) {
      /* I2C is not usable at this point. Releasing the mutex would allow the
       * usage which could lead to undefined behaviour. */
      return sc;
    }

    sc = rtems_semaphore_release(e->mutex);
    assert(sc == RTEMS_SUCCESSFUL);
  }

  return sc;
}

rtems_device_driver i2cdrv_open(
  rtems_device_major_number major,
  rtems_device_major_number minor,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  i2cdrv_entry *e = &i2cdrv_table[minor];

  sc = rtems_semaphore_obtain(e->mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  return sc;
}

rtems_device_driver i2cdrv_close(
  rtems_device_major_number major,
  rtems_device_major_number minor,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  i2cdrv_entry *e = &i2cdrv_table[minor];

  sc = rtems_semaphore_release(e->mutex);
  return sc;
}

rtems_device_driver i2cdrv_read(
  rtems_device_major_number major,
  rtems_device_major_number minor,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  i2cdrv_entry *e = &i2cdrv_table[minor];
  rtems_libio_rw_args_t *rw  = arg;
  ALT_I2C_DEV_t *dev = get_device(e);
  ALT_STATUS_CODE asc = ALT_E_SUCCESS;

  asc = alt_i2c_master_receive(dev, rw->buffer, rw->count, true, true);
  if ( asc == ALT_E_SUCCESS ) {
    rw->bytes_moved = rw->count;
  } else {
    sc = RTEMS_IO_ERROR;
  }

  return sc;
}

rtems_device_driver i2cdrv_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  i2cdrv_entry *e = &i2cdrv_table[minor];
  rtems_libio_rw_args_t *rw  = arg;
  ALT_I2C_DEV_t *dev = get_device(e);
  ALT_STATUS_CODE asc = ALT_E_SUCCESS;

  asc = alt_i2c_master_transmit(dev, rw->buffer, rw->count, true, true);
  if ( asc == ALT_E_SUCCESS ) {
    rw->bytes_moved = rw->count;
  } else {
    sc = RTEMS_IO_ERROR;
  }

  return sc;
}

static rtems_status_code ioctl_set_slave_address(
  i2cdrv_entry *e,
  rtems_libio_ioctl_args_t *args
)
{
  ALT_I2C_DEV_t *dev = get_device(e);
  ALT_STATUS_CODE asc = ALT_E_SUCCESS;
  uint32_t address = (uint32_t) args->buffer;

  asc = alt_i2c_master_target_set(dev, address);
  if ( asc != ALT_E_SUCCESS ) {
    return RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver i2cdrv_ioctl(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  i2cdrv_entry *e = &i2cdrv_table[minor];
  rtems_libio_ioctl_args_t *args = arg;

  switch (args->command) {
    case I2C_IOC_SET_SLAVE_ADDRESS:
      sc = ioctl_set_slave_address(e, args);
      break;
    default:
      sc = RTEMS_INVALID_NUMBER;
      break;
  }

  return sc;
}
