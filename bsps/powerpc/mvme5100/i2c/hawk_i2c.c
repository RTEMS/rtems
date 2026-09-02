/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME5100
 *
 * @brief Polled I2C bus driver for the Hawk ASIC
 */

/*
 * Copyright (C) 2026 UChicago Argonne LLC,
 * as operator of Argonne National Laboratory
 * Author: Vijay Banerjee <vijay@rtems.org>
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
#include <bsp/hawk_i2c_busdrv.h>
#include <libcpu/io.h>
#include <rtems.h>
#include <rtems/libi2c.h>

#define HAWK_I2C_PRESCALE ((volatile uint32_t *)(BSP_HAWK_SMC_BASE + 0x90))
#define HAWK_I2C_CONTROL ((volatile uint32_t *)(BSP_HAWK_SMC_BASE + 0x98))
#define HAWK_I2C_STATUS ((volatile uint32_t *)(BSP_HAWK_SMC_BASE + 0xa0))
#define HAWK_I2C_DATAWR ((volatile uint32_t *)(BSP_HAWK_SMC_BASE + 0xa8))
#define HAWK_I2C_DATARD ((volatile uint32_t *)(BSP_HAWK_SMC_BASE + 0xb0))

#define I2_START 0x08
#define I2_STOP 0x04
#define I2_ACKOUT 0x02
#define I2_ENBL 0x01

#define I2_DATIN 0x08
#define I2_ERR 0x04
#define I2_ACKIN 0x02
#define I2_CMPLT 0x01

#define HAWK_I2C_TIMEOUT 10000000

static uint32_t hawk_i2c_tb(void) {
  uint32_t tb;

  __asm__ volatile("mftb %0" : "=r"(tb));
  return tb;
}

static rtems_status_code hawk_i2c_wait(uint32_t mask, uint32_t *statp) {
  uint32_t then = hawk_i2c_tb();
  uint32_t stat;

  do {
    stat = in_be32(HAWK_I2C_STATUS);
    if ((stat & mask) == mask) {
      if (statp != NULL) {
        *statp = stat;
      }
      return RTEMS_SUCCESSFUL;
    }
    if (stat & I2_ERR) {
      return RTEMS_IO_ERROR;
    }
  } while (hawk_i2c_tb() - then < HAWK_I2C_TIMEOUT);

  return RTEMS_TIMEOUT;
}

static rtems_status_code hawk_i2c_xfer(uint32_t data, uint32_t mask) {
  out_be32(HAWK_I2C_DATAWR, data);
  return hawk_i2c_wait(mask, NULL);
}

static rtems_status_code hawk_i2c_init(rtems_libi2c_bus_t *bh) {
  (void)bh;

  return hawk_i2c_wait(I2_CMPLT, NULL);
}

/* the Hawk emits the start on the next DATAWR write */
static rtems_status_code hawk_i2c_start(rtems_libi2c_bus_t *bh) {
  (void)bh;

  return hawk_i2c_wait(I2_CMPLT, NULL);
}

static rtems_status_code hawk_i2c_stop(rtems_libi2c_bus_t *bh) {
  (void)bh;

  out_be32(HAWK_I2C_CONTROL, I2_STOP | I2_ENBL);
  return hawk_i2c_xfer(0, I2_CMPLT);
}

static rtems_status_code hawk_i2c_send_addr(rtems_libi2c_bus_t *bh,
                                            uint32_t addr, int rw) {
  (void)bh;

  out_be32(HAWK_I2C_CONTROL, I2_START | I2_ENBL);
  return hawk_i2c_xfer(((addr << 1) | (rw ? 1 : 0)) & 0xff,
                       I2_CMPLT | I2_ACKIN);
}

static int hawk_i2c_write_bytes(rtems_libi2c_bus_t *bh, unsigned char *bytes,
                                int nbytes) {
  int i;

  (void)bh;

  for (i = 0; i < nbytes; i++) {
    if (hawk_i2c_xfer(bytes[i], I2_CMPLT | I2_ACKIN) != RTEMS_SUCCESSFUL) {
      return -RTEMS_IO_ERROR;
    }
  }
  return nbytes;
}

static int hawk_i2c_read_bytes(rtems_libi2c_bus_t *bh, unsigned char *bytes,
                               int nbytes) {
  int i;

  (void)bh;

  for (i = 0; i < nbytes; i++) {

    /* acknowledge every byte but the last one */
    out_be32(HAWK_I2C_CONTROL,
             (i + 1 < nbytes) ? (I2_ACKOUT | I2_ENBL) : I2_ENBL);

    if (hawk_i2c_xfer(0, I2_CMPLT | I2_DATIN) != RTEMS_SUCCESSFUL) {
      return -RTEMS_IO_ERROR;
    }

    bytes[i] = in_be32(HAWK_I2C_DATARD) & 0xff;
  }
  return nbytes;
}

static rtems_libi2c_bus_ops_t hawk_i2c_ops = {
    .init = hawk_i2c_init,
    .send_start = hawk_i2c_start,
    .send_stop = hawk_i2c_stop,
    .send_addr = hawk_i2c_send_addr,
    .read_bytes = hawk_i2c_read_bytes,
    .write_bytes = hawk_i2c_write_bytes,
};

static rtems_libi2c_bus_t hawk_i2c_bus = {
    .ops = &hawk_i2c_ops,
    .size = sizeof(hawk_i2c_bus),
};

rtems_libi2c_bus_t *hawk_i2c_bus_descriptor = &hawk_i2c_bus;
