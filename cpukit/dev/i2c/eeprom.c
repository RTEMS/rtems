/**
 * @file
 *
 * @brief EEPROM Driver Implementation
 *
 * @ingroup I2CEEPROM
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <dev/i2c/eeprom.h>

#include <string.h>

#define EEPROM_MAX_ADDRESS_BYTES 4

#define EEPROM_MAX_PAGE_SIZE 128

typedef struct {
  i2c_dev base;
  uint16_t address_bytes;
  uint16_t page_size;
  uint32_t size;
  uint16_t i2c_address_mask;
  uint16_t i2c_address_shift;
  rtems_interval program_timeout_in_ticks;
} eeprom;

static uint16_t eeprom_i2c_addr(eeprom *dev, uint32_t off)
{
  return dev->base.address
    | ((off >> dev->i2c_address_shift) & dev->i2c_address_mask);
}

static void eeprom_set_addr(
  const eeprom *dev,
  uint32_t off,
  uint8_t addr[EEPROM_MAX_ADDRESS_BYTES]
)
{
  int shift = 24 - (4 - dev->address_bytes) * 8;

  addr[0] = (uint8_t) (off >> shift);
  shift -= 8;
  addr[1] = (uint8_t) (off >> shift);
  shift -= 8;
  addr[2] = (uint8_t) (off >> shift);
  shift -= 8;
  addr[3] = (uint8_t) (off >> shift);
}

static ssize_t eeprom_read(
  i2c_dev *base,
  void *buf,
  size_t n,
  off_t offset
)
{
  eeprom *dev = (eeprom *) base;
  off_t avail = dev->size - offset;
  uint32_t off = (uint32_t) offset;
  uint8_t *in = buf;
  size_t todo;

  if (avail <= 0) {
    return 0;
  }

  if (n > avail) {
    n = (size_t) avail;
  }

  todo = n;

  while (todo > 0) {
    uint16_t i2c_addr = eeprom_i2c_addr(dev, off);

    /*
     * Limit the transfer size so that it can be stored in 8-bits.  This may
     * help some bus controllers.
     */
    uint16_t cur = (uint16_t) (todo < 255 ?  todo : 255);

    uint8_t addr[EEPROM_MAX_ADDRESS_BYTES];
    i2c_msg msgs[2] = {
      {
        .addr = i2c_addr,
        .flags = 0,
        .len = dev->address_bytes,
        .buf = &addr[0]
      }, {
        .addr = i2c_addr,
        .flags = I2C_M_RD,
        .buf = in,
        .len = cur
      }
    };
    int err;

    eeprom_set_addr(dev, off, addr);
    err = i2c_bus_transfer(dev->base.bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
    if (err != 0) {
      return err;
    }

    todo -= cur;
    off += cur;
    in += cur;
  }

  return (ssize_t) n;
}

static ssize_t eeprom_write(
  i2c_dev *base,
  const void *buf,
  size_t n,
  off_t offset
)
{
  eeprom *dev = (eeprom *) base;
  off_t avail = dev->size - offset;
  uint32_t off = (uint32_t) offset;
  const uint8_t *out = buf;
  size_t todo;

  if (avail <= 0) {
    return 0;
  }

  if (n > avail) {
    n = (size_t) avail;
  }

  todo = n;

  while (todo > 0) {
    uint16_t i2c_addr = eeprom_i2c_addr(dev, off);
    uint16_t rem = dev->page_size - (off & (dev->page_size - 1));
    uint16_t cur = (uint16_t) (todo < rem ? todo : rem);
    uint8_t addr[EEPROM_MAX_ADDRESS_BYTES];
    i2c_msg msgs[2] = {
      {
        .addr = i2c_addr,
        .flags = 0,
        .len = dev->address_bytes,
        .buf = &addr[0]
      }, {
        .addr = i2c_addr,
        .flags = I2C_M_NOSTART,
        .buf = RTEMS_DECONST(uint8_t *, out),
        .len = cur
      }
    };
    uint8_t in[EEPROM_MAX_PAGE_SIZE];
    int err;
    ssize_t m;
    rtems_interval timeout;
    bool before;

    eeprom_set_addr(dev, off, addr);
    err = i2c_bus_transfer(dev->base.bus, &msgs[0], RTEMS_ARRAY_SIZE(msgs));
    if (err != 0) {
      return err;
    }

    timeout = rtems_clock_tick_later(dev->program_timeout_in_ticks);

    do {
      before = rtems_clock_tick_before(timeout);

      m = eeprom_read(&dev->base, &in[0], cur, off);
      if (m == cur) {
        break;
      }
    } while (before);

    if (m != cur) {
      return -ETIMEDOUT;
    }

    if (memcmp(&in[0], &out[0], cur) != 0) {
      return -EIO;
    }

    todo -= cur;
    off += cur;
    out += cur;
  }

  return (ssize_t) n;
}

static off_t eeprom_get_size(i2c_dev *base)
{
  eeprom *dev = (eeprom *) base;

  return dev->size;
}

static blksize_t eeprom_get_block_size(i2c_dev *base)
{
  eeprom *dev = (eeprom *) base;

  return dev->page_size;
}

int i2c_dev_register_eeprom(
  const char *bus_path,
  const char *dev_path,
  uint16_t i2c_address,
  uint16_t address_bytes,
  uint16_t page_size_in_bytes,
  uint32_t size_in_bytes,
  uint32_t program_timeout_in_ms
)
{
  uint32_t extra_address;
  uint32_t ms_per_tick;
  eeprom *dev;

  if (address_bytes > EEPROM_MAX_ADDRESS_BYTES) {
    rtems_set_errno_and_return_minus_one(ERANGE);
  } else if (address_bytes == EEPROM_MAX_ADDRESS_BYTES) {
    extra_address = 0;
  } else {
    extra_address = size_in_bytes >> (8 * address_bytes);
  }

  if (extra_address != 0 && (extra_address & (extra_address - 1)) != 0) {
    rtems_set_errno_and_return_minus_one(EINVAL);
  }

  if (page_size_in_bytes > EEPROM_MAX_PAGE_SIZE) {
    page_size_in_bytes = EEPROM_MAX_PAGE_SIZE;
  }

  if (program_timeout_in_ms == 0) {
    program_timeout_in_ms = 1000;
  }

  dev = (eeprom *)
    i2c_dev_alloc_and_init(sizeof(*dev), bus_path, i2c_address);
  if (dev == NULL) {
    return -1;
  }

  dev->base.read = eeprom_read;
  dev->base.write = eeprom_write;
  dev->base.get_size = eeprom_get_size;
  dev->base.get_block_size = eeprom_get_block_size;
  dev->address_bytes = address_bytes;
  dev->page_size = page_size_in_bytes;
  dev->size = size_in_bytes;
  ms_per_tick = rtems_configuration_get_milliseconds_per_tick();
  dev->program_timeout_in_ticks = (program_timeout_in_ms + ms_per_tick - 1)
    / ms_per_tick + 1;

  if (extra_address != 0) {
    dev->i2c_address_mask = extra_address - 1;
    dev->i2c_address_shift = (uint16_t) (8 * address_bytes);
  }

  return i2c_dev_register(&dev->base, dev_path);
}
