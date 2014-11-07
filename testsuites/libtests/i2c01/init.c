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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <dev/i2c/i2c.h>
#include <dev/i2c/eeprom.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems/libcsupport.h>

#include "tmacros.h"

const char rtems_test_name[] = "I2C 1";

#define SPARE_ADDRESS_BITS 3

#define DEVICE_SIMPLE_READ_WRITE (0UL << SPARE_ADDRESS_BITS)

#define DEVICE_EEPROM (1UL << SPARE_ADDRESS_BITS)

#define EEPROM_SIZE 512

typedef struct test_device test_device;

struct test_device {
  int (*transfer)(
    i2c_bus *bus,
    i2c_msg *msgs,
    uint32_t msg_count,
    test_device *dev
  );
};

typedef struct {
  test_device base;
  char buf[3];
} test_device_simple_read_write;

typedef struct {
  test_device base;
  unsigned current_address;
  uint8_t data[EEPROM_SIZE];
} test_device_eeprom;

typedef struct {
  i2c_bus base;
  unsigned long clock;
  test_device *devices[2];
  test_device_simple_read_write simple_read_write;
  test_device_eeprom eeprom;
} test_bus;

static const char bus_path[] = "/dev/i2c-0";

static const char eeprom_path[] = "/dev/i2c-0.eeprom-0";

static void cyclic_inc(unsigned *val, unsigned cycle)
{
  unsigned v = *val;
  unsigned m = cycle - 1U;

  *val = (v & ~m) + ((v + 1U) & m);
}

static int test_simple_read_write_transfer(
  i2c_bus *bus,
  i2c_msg *msgs,
  uint32_t msg_count,
  test_device *base
)
{
  test_device_simple_read_write *dev = (test_device_simple_read_write *) base;

  if (msg_count == 1 && msgs[0].len == sizeof(dev->buf)) {
    if ((msgs[0].flags & I2C_M_RD) != 0) {
      memcpy(msgs[0].buf, &dev->buf[0], sizeof(dev->buf));
    } else {
      memcpy(&dev->buf[0], msgs[0].buf, sizeof(dev->buf));
    }

    return 0;
  } else {
    return -EIO;
  }
}

static int test_eeprom_transfer(
  i2c_bus *bus,
  i2c_msg *msgs,
  uint32_t msg_count,
  test_device *base
)
{
  test_device_eeprom *dev = (test_device_eeprom *) base;
  i2c_msg *msg = &msgs[0];
  uint32_t i;

  if (msg_count > 0 && (msg->flags & I2C_M_RD) == 0) {
    if (msg->len < 1) {
      return -EIO;
    }

    dev->current_address = msg->buf[0] | ((msg->addr & 0x1) << 8);
    --msg->len;
    ++msg->buf;
  }

  for (i = 0; i < msg_count; ++i) {
    int j;

    msg = &msgs[i];

    if ((msg->flags & I2C_M_RD) != 0) {
      for (j = 0; j < msg->len; ++j) {
        msg->buf[j] = dev->data[dev->current_address];
        cyclic_inc(&dev->current_address, sizeof(dev->data));
      }
    } else {
      for (j = 0; j < msg->len; ++j) {
        dev->data[dev->current_address] = msg->buf[j];
        cyclic_inc(&dev->current_address, 8);
      }
    }
  }

  return 0;
}

static int test_transfer(i2c_bus *base, i2c_msg *msgs, uint32_t msg_count)
{
  test_bus *bus = (test_bus *) base;
  uint16_t addr;
  test_device *dev;

  addr = msgs[0].addr >> SPARE_ADDRESS_BITS;
  if (addr >= RTEMS_ARRAY_SIZE(bus->devices)) {
    return -EIO;
  }

  dev = bus->devices[addr];

  return (*dev->transfer)(&bus->base, msgs, msg_count, dev);
}

static int test_set_clock(i2c_bus *base, unsigned long clock)
{
  test_bus *bus = (test_bus *) base;

  bus->clock = clock;

  return 0;
}

static void test_destroy(i2c_bus *base)
{
  i2c_bus_destroy_and_free(base);
}

static void test_simple_read_write(test_bus *bus, int fd)
{
  static const char zero[] = { 0, 0, 0 };
  static const char abc[] = { 'a', 'b', 'c' };

  int rv;
  char buf[3];
  ssize_t n;

  rv = ioctl(fd, I2C_SLAVE, DEVICE_SIMPLE_READ_WRITE);
  rtems_test_assert(rv == 0);

  errno = 0;
  rv = ioctl(fd, 0xb00b);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOTTY);

  errno = 0;
  n = write(fd, &buf[0], 1000);
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == EIO);

  errno = 0;
  n = read(fd, &buf[0], 1000);
  rtems_test_assert(n == -1);
  rtems_test_assert(errno == EIO);

  rtems_test_assert(
    memcmp(&bus->simple_read_write.buf[0], &zero[0], sizeof(buf)) == 0
  );

  n = write(fd, &abc[0], sizeof(buf));
  rtems_test_assert(n == (ssize_t) sizeof(buf));

  rtems_test_assert(
    memcmp(&bus->simple_read_write.buf[0], &abc[0], sizeof(buf)) == 0
  );

  n = read(fd, &buf[0], sizeof(buf));
  rtems_test_assert(n == (ssize_t) sizeof(buf));

  rtems_test_assert(memcmp(&buf[0], &abc[0], sizeof(buf)) == 0);
}

static void test_eeprom(void)
{
  int rv;
  int fd_in;
  int fd_out;
  struct stat st;
  uint8_t in[EEPROM_SIZE];
  uint8_t out[EEPROM_SIZE];
  ssize_t n;
  off_t off;
  size_t i;

  rv = i2c_dev_register_eeprom(
    &bus_path[0],
    &eeprom_path[0],
    DEVICE_EEPROM,
    1,
    8,
    sizeof(out),
    0
  );
  rtems_test_assert(rv == 0);

  fd_in = open(&eeprom_path[0], O_RDWR);
  rtems_test_assert(fd_in >= 0);

  fd_out = open(&eeprom_path[0], O_RDWR);
  rtems_test_assert(fd_out >= 0);

  rv = fstat(fd_in, &st);
  rtems_test_assert(rv == 0);
  rtems_test_assert(st.st_blksize == 8);
  rtems_test_assert(st.st_size == sizeof(out));

  memset(&out[0], 0, sizeof(out));

  n = read(fd_in, &in[0], sizeof(in) + 1);
  rtems_test_assert(n == (ssize_t) sizeof(in));

  rtems_test_assert(memcmp(&in[0], &out[0], sizeof(in)) == 0);

  off = lseek(fd_in, 0, SEEK_CUR);
  rtems_test_assert(off == sizeof(out));

  for (i = 0; i < sizeof(out); ++i) {
    off = lseek(fd_out, 0, SEEK_CUR);
    rtems_test_assert(off == i);

    out[i] = (uint8_t) i;

    n = write(fd_out, &out[i], sizeof(out[i]));
    rtems_test_assert(n == (ssize_t) sizeof(out[i]));

    off = lseek(fd_in, 0, SEEK_SET);
    rtems_test_assert(off == 0);

    n = read(fd_in, &in[0], sizeof(in));
    rtems_test_assert(n == (ssize_t) sizeof(in));

    rtems_test_assert(memcmp(&in[0], &out[0], sizeof(in)) == 0);
  }

  rv = close(fd_in);
  rtems_test_assert(rv == 0);

  rv = close(fd_out);
  rtems_test_assert(rv == 0);

  rv = unlink(&eeprom_path[0]);
  rtems_test_assert(rv == 0);
}

static void test(void)
{
  rtems_resource_snapshot snapshot;
  test_bus *bus;
  int rv;
  int fd;

  rtems_resource_snapshot_take(&snapshot);

  bus = (test_bus *) i2c_bus_alloc_and_init(sizeof(*bus));
  rtems_test_assert(bus != NULL);

  bus->base.transfer = test_transfer;
  bus->base.set_clock = test_set_clock;
  bus->base.destroy = test_destroy;
  bus->base.functionality = I2C_FUNC_I2C | I2C_FUNC_PROTOCOL_MANGLING
    | I2C_FUNC_NOSTART;

  bus->simple_read_write.base.transfer = test_simple_read_write_transfer;
  bus->devices[0] = &bus->simple_read_write.base;

  bus->eeprom.base.transfer = test_eeprom_transfer;
  bus->devices[1] = &bus->eeprom.base;

  rv = i2c_bus_register(&bus->base, &bus_path[0]);
  rtems_test_assert(rv == 0);

  fd = open(&bus_path[0], O_RDWR);
  rtems_test_assert(fd >= 0);

  rtems_test_assert(bus->clock == 0);
  rv = ioctl(fd, I2C_BUS_SET_CLOCK, 0xdeadbeefUL);
  rtems_test_assert(rv == 0);
  rtems_test_assert(bus->clock == 0xdeadbeef);

  rv = ioctl(fd, I2C_BUS_OBTAIN);
  rtems_test_assert(rv == 0);

  rv = ioctl(fd, I2C_BUS_RELEASE);
  rtems_test_assert(rv == 0);

  rtems_test_assert(!bus->base.ten_bit_address);

  rv = ioctl(fd, I2C_TENBIT, 1UL);
  rtems_test_assert(rv == 0);
  rtems_test_assert(bus->base.ten_bit_address);

  rv = ioctl(fd, I2C_TENBIT, 0UL);
  rtems_test_assert(rv == 0);
  rtems_test_assert(!bus->base.ten_bit_address);

  rtems_test_assert(!bus->base.use_pec);

  rv = ioctl(fd, I2C_PEC, 1UL);
  rtems_test_assert(rv == 0);
  rtems_test_assert(bus->base.use_pec);

  rv = ioctl(fd, I2C_PEC, 0UL);
  rtems_test_assert(rv == 0);
  rtems_test_assert(!bus->base.use_pec);

  rv = ioctl(fd, I2C_SLAVE, 123UL);
  rtems_test_assert(rv == 0);
  rtems_test_assert(bus->base.default_address == 123);

  rv = ioctl(fd, I2C_SLAVE_FORCE, 456UL);
  rtems_test_assert(rv == 0);
  rtems_test_assert(bus->base.default_address == 456);

  rtems_test_assert(bus->base.retries == 0);

  rv = ioctl(fd, I2C_RETRIES, 1UL);
  rtems_test_assert(rv == 0);
  rtems_test_assert(bus->base.retries == 1);

  rv = ioctl(fd, I2C_RETRIES, 0UL);
  rtems_test_assert(rv == 0);
  rtems_test_assert(bus->base.retries == 0);

  rtems_test_assert(bus->base.timeout == 0);

  rv = ioctl(fd, I2C_TIMEOUT, 1UL);
  rtems_test_assert(rv == 0);
  rtems_test_assert(bus->base.timeout == 5);

  rv = ioctl(fd, I2C_TIMEOUT, 0UL);
  rtems_test_assert(rv == 0);
  rtems_test_assert(bus->base.timeout == 0);

  test_simple_read_write(bus, fd);
  test_eeprom();

  rv = close(fd);
  rtems_test_assert(rv == 0);

  rv = unlink(&bus_path[0]);
  rtems_test_assert(rv == 0);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 2000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 7

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
