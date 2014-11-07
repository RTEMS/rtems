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
  i2c_bus base;
  unsigned long clock;
  test_device *devices[1];
  test_device_simple_read_write simple_read_write;
} test_bus;

static const char bus_path[] = "/dev/i2c-0";

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
