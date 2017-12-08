/**
 * @file
 *
 * @brief Inter-Integrated Circuit (I2C) Bus Implementation
 *
 * @ingroup I2CBus
 */

/*
 * Copyright (c) 2014, 2017 embedded brains GmbH.  All rights reserved.
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

#include <dev/i2c/i2c.h>

#include <rtems/imfs.h>

#include <stdlib.h>
#include <string.h>

void i2c_bus_obtain(i2c_bus *bus)
{
  rtems_recursive_mutex_lock(&bus->mutex);
}

void i2c_bus_release(i2c_bus *bus)
{
  rtems_recursive_mutex_unlock(&bus->mutex);
}

int i2c_bus_transfer(i2c_bus *bus, i2c_msg *msgs, uint32_t msg_count)
{
  int err;
  uint32_t i;
  uint32_t j;

  _Assert(msg_count > 0);

  for (i = 0, j = 0; i < msg_count; ++i) {
    if ((msgs[i].flags & I2C_M_NOSTART) != 0) {
      if ((msgs[i].flags & I2C_M_RD) != (msgs[j].flags & I2C_M_RD)) {
        return -EINVAL;
      }

      if (msgs[i].addr != msgs[j].addr) {
        return -EINVAL;
      }
    } else {
      j = i;
    }
  }

  i2c_bus_obtain(bus);
  err = (*bus->transfer)(bus, msgs, msg_count);
  i2c_bus_release(bus);

  return err;
}

static ssize_t i2c_bus_read(
  rtems_libio_t *iop,
  void *buffer,
  size_t count
)
{
  i2c_bus *bus = IMFS_generic_get_context_by_iop(iop);
  i2c_msg msg = {
    .addr = bus->default_address,
    .flags = I2C_M_RD,
    .len = (uint16_t) count,
    .buf = buffer
  };
  int err;

  if (bus->ten_bit_address) {
    msg.flags |= I2C_M_TEN;
  }

  err = i2c_bus_transfer(bus, &msg, 1);
  if (err == 0) {
    return msg.len;
  } else {
    rtems_set_errno_and_return_minus_one(-err);
  }
}

static ssize_t i2c_bus_write(
  rtems_libio_t *iop,
  const void *buffer,
  size_t count
)
{
  i2c_bus *bus = IMFS_generic_get_context_by_iop(iop);
  i2c_msg msg = {
    .addr = bus->default_address,
    .flags = 0,
    .len = (uint16_t) count,
    .buf = RTEMS_DECONST(void *, buffer)
  };
  int err;

  if (bus->ten_bit_address) {
    msg.flags |= I2C_M_TEN;
  }

  err = i2c_bus_transfer(bus, &msg, 1);
  if (err == 0) {
    return msg.len;
  } else {
    rtems_set_errno_and_return_minus_one(-err);
  }
}

static int i2c_bus_ioctl(
  rtems_libio_t *iop,
  ioctl_command_t command,
  void *arg
)
{
  i2c_bus *bus = IMFS_generic_get_context_by_iop(iop);
  i2c_rdwr_ioctl_data *rdwr;
  int err;

  switch (command) {
    case I2C_RDWR:
      rdwr = arg;
      if (rdwr->nmsgs > 0) {
        err = i2c_bus_transfer(bus, rdwr->msgs, rdwr->nmsgs);
      } else {
        err = 0;
      }
      break;
    case I2C_BUS_OBTAIN:
      i2c_bus_obtain(bus);
      err = 0;
      break;
    case I2C_BUS_RELEASE:
      i2c_bus_release(bus);
      err = 0;
      break;
    case I2C_BUS_GET_CONTROL:
      *(i2c_bus **) arg = bus;
      err = 0;
      break;
    case I2C_FUNCS:
      *(unsigned long *) arg = bus->functionality;
      err = 0;
      break;
    case I2C_RETRIES:
      bus->retries = (unsigned long) arg;
      err = 0;
      break;
    case I2C_TIMEOUT:
      bus->timeout = RTEMS_MILLISECONDS_TO_TICKS(10 * (unsigned long) arg);
      err = 0;
      break;
    case I2C_SLAVE:
    case I2C_SLAVE_FORCE:
      bus->default_address = (unsigned long) arg;
      err = 0;
      break;
    case I2C_TENBIT:
      bus->ten_bit_address = (unsigned long) arg != 0;
      err = 0;
      break;
    case I2C_PEC:
      bus->use_pec = (unsigned long) arg != 0;
      err = 0;
      break;
    case I2C_BUS_SET_CLOCK:
      i2c_bus_obtain(bus);
      err = (*bus->set_clock)(bus, (unsigned long) arg);
      i2c_bus_release(bus);
      break;
    default:
      err = -ENOTTY;
      break;
  }

  if (err == 0) {
    return 0;
  } else {
    rtems_set_errno_and_return_minus_one(-err);
  }
}

static const rtems_filesystem_file_handlers_r i2c_bus_handler = {
  .open_h = rtems_filesystem_default_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = i2c_bus_read,
  .write_h = i2c_bus_write,
  .ioctl_h = i2c_bus_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static void i2c_bus_node_destroy(IMFS_jnode_t *node)
{
  i2c_bus *bus;

  bus = IMFS_generic_get_context_by_node(node);
  (*bus->destroy)(bus);

  IMFS_node_destroy_default(node);
}

static const IMFS_node_control i2c_bus_node_control = IMFS_GENERIC_INITIALIZER(
  &i2c_bus_handler,
  IMFS_node_initialize_generic,
  i2c_bus_node_destroy
);

int i2c_bus_register(
  i2c_bus *bus,
  const char *bus_path
)
{
  int rv;

  rv = IMFS_make_generic_node(
    bus_path,
    S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
    &i2c_bus_node_control,
    bus
  );
  if (rv != 0) {
    (*bus->destroy)(bus);
  }

  return rv;
}

static int i2c_bus_transfer_default(
  i2c_bus *bus,
  i2c_msg *msgs,
  uint32_t msg_count
)
{
  (void) bus;
  (void) msgs;
  (void) msg_count;

  return -EIO;
}

static int i2c_bus_set_clock_default(i2c_bus *bus, unsigned long clock)
{
  (void) bus;
  (void) clock;

  return -EIO;
}

static int i2c_bus_do_init(
  i2c_bus *bus,
  void (*destroy)(i2c_bus *bus)
)
{
  rtems_recursive_mutex_init(&bus->mutex, "I2C Bus");
  bus->transfer = i2c_bus_transfer_default;
  bus->set_clock = i2c_bus_set_clock_default;
  bus->destroy = destroy;

  return 0;
}

void i2c_bus_destroy(i2c_bus *bus)
{
  rtems_recursive_mutex_destroy(&bus->mutex);
}

void i2c_bus_destroy_and_free(i2c_bus *bus)
{
  i2c_bus_destroy(bus);
  free(bus);
}

int i2c_bus_init(i2c_bus *bus)
{
  memset(bus, 0, sizeof(*bus));

  return i2c_bus_do_init(bus, i2c_bus_destroy);
}

i2c_bus *i2c_bus_alloc_and_init(size_t size)
{
  i2c_bus *bus = NULL;

  if (size >= sizeof(*bus)) {
    bus = calloc(1, size);
    if (bus != NULL) {
      int rv;

      rv = i2c_bus_do_init(bus, i2c_bus_destroy_and_free);
      if (rv != 0) {
        return NULL;
      }
    }
  }

  return bus;
}
