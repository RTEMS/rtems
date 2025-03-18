/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016-2017 Chris Johns <chrisj@rtems.org>
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

/*
 * I2C slave for testing:
 *       https://github.com/oetr/FPGA-I2C-Slave
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dev/i2c/i2c.h>
#include <dev/i2c/fpga-i2c-slave.h>

typedef struct {
  i2c_dev  base;
  uint16_t address;
  size_t   size;
} fpga_i2c_slave;

static ssize_t
fpga_i2c_slave_read(i2c_dev* base, void* buf, size_t n, off_t offset)
{
  fpga_i2c_slave* dev = (fpga_i2c_slave*) base;
  off_t           avail = dev->size - offset;
  uint8_t*        in = buf;
  size_t          todo;

  if (avail <= 0) {
    return 0;
  }

  if (n > avail) {
    n = (size_t) avail;
  }

  todo = n;

  while (todo > 0) {
    /*
     * Limit the transfer size so that it can be stored in 8-bits.  This may
     * help some bus controllers.
     */
    uint16_t cur = (uint16_t) (todo < 255 ?  todo : 255);
    i2c_msg msgs = {
      .addr = dev->base.address,
      .flags = I2C_M_RD,
      .buf = in,
      .len = cur
    };
    int err;
    err = i2c_bus_transfer(dev->base.bus, &msgs, 1);
    if (err != 0) {
      return err;
    }
    todo -= cur;
    in += cur;
  }

  return (ssize_t) n;
}

static ssize_t
fpga_i2c_slave_write(i2c_dev* base, const void* buf, size_t n, off_t offset)
{
  fpga_i2c_slave* dev = (fpga_i2c_slave*) base;
  off_t           avail = dev->size - offset;
  const uint8_t*  out = buf;
  size_t          todo;

  if (avail <= 0) {
    return 0;
  }

  if (n > avail) {
    n = (size_t) avail;
  }

  todo = n;

  while (todo > 0) {
    /*
     * Limit the transfer size so that it can be stored in 8-bits.  This may
     * help some bus controllers.
     */
    uint16_t cur = (uint16_t) (todo < 255 ?  todo : 255);
    i2c_msg msgs = {
      .addr = dev->base.address,
      .flags = 0,
      .buf = RTEMS_DECONST(uint8_t*, out),
      .len = cur
    };
    int err;
    err = i2c_bus_transfer(dev->base.bus, &msgs, 1);
    if (err != 0) {
      return err;
    }
    todo -= cur;
    out += cur;
  }

  return (ssize_t) n;
}

int
i2c_dev_register_fpga_i2c_slave(const char* bus_path,
                                const char* dev_path,
                                uint16_t    address,
                                size_t      size)

{
  fpga_i2c_slave* dev;

  dev = (fpga_i2c_slave*)
    i2c_dev_alloc_and_init(sizeof(*dev), bus_path, address);
  if (dev == NULL) {
    return -1;
  }

  dev->base.read = fpga_i2c_slave_read;
  dev->base.write = fpga_i2c_slave_write;
  dev->size = size;

  return i2c_dev_register(&dev->base, dev_path);
}
