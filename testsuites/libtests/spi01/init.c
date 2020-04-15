/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <dev/spi/spi.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems/libcsupport.h>

#include "tmacros.h"

static uint8_t mode_8 = 0xA5;
static uint32_t mode_32 = 0x5A5A5A5A;
static uint32_t speed = 12345678;
static uint8_t bits_per_word = 12;
static uint8_t lsb_first = 1;

const char rtems_test_name[] = "SPI 1";

static const char bus_path[] = "/dev/spi-0";

typedef struct test_device test_device;

struct test_device {
  int (*transfer)(
    spi_bus *bus,
    const spi_ioc_transfer *msgs,
    uint32_t msg_count,
    test_device *dev
  );
};

typedef struct {
  test_device base;
  char buf[3];
} test_device_simple_read_write;

typedef struct {
  spi_bus base;
  unsigned long clock;
  test_device *device;
  uint32_t msg_count;
  uint32_t max_speed_hz;
  test_device_simple_read_write simple_read_write;
} test_bus;

static int test_simple_read_write_transfer(
  spi_bus *bus,
  const spi_ioc_transfer *msgs,
  uint32_t msg_count,
  test_device *base
)
{
  (void)bus;

  test_device_simple_read_write *dev = (test_device_simple_read_write *) base;

  if (msg_count == 1 && msgs[0].len == sizeof(dev->buf)) {
    if (msgs[0].rx_buf == 0){
        memcpy(&dev->buf[0], msgs[0].tx_buf, sizeof(dev->buf));
    } else if (msgs[0].tx_buf == 0){
        memcpy(msgs[0].rx_buf, &dev->buf[0], sizeof(dev->buf));
    } else {
        return -EIO;
    }
  } else {
    return -EIO;
  }
  return 0;
}

static int test_transfer(
  spi_bus *base,
  const spi_ioc_transfer *msgs,
  uint32_t msg_count
)
{
  test_bus *bus = (test_bus *) base;
  test_device *dev;

  dev = bus->device;
  bus->msg_count = msg_count;

  return (*dev->transfer)(&bus->base, msgs, msg_count, dev);
}

static int test_setup(spi_bus *base)
{
  test_bus *bus = (test_bus *) base;

  if ((base->speed_hz > bus->max_speed_hz) ||
    ((base->bits_per_word < 8) || (base->bits_per_word > 16))) {
    return 1;
  }

  return 0;
}

static void test_destroy(spi_bus *base)
{
  spi_bus_destroy_and_free(base);
}

static void test_simple_read_write(test_bus *bus, int fd)
{
  static const char zero[] = { 0, 0, 0 };
  static const char abc[] = { 'a', 'b', 'c' };

  int rv;
  char buf[3];
  ssize_t n;

  errno = 0;
  rv = ioctl(fd, 0xb00b);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == EINVAL);

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
  uint8_t read_mode_8;
  uint32_t read_mode_32;
  uint32_t read_speed;
  uint8_t read_bits_per_word;
  uint8_t read_lsb_first;
  spi_ioc_transfer msg;

  rtems_resource_snapshot_take(&snapshot);

  bus = (test_bus *) spi_bus_alloc_and_init(sizeof(*bus));
  rtems_test_assert(bus != NULL);

  bus->base.transfer = test_transfer;
  bus->base.destroy = test_destroy;
  bus->base.setup = test_setup;

  bus->simple_read_write.base.transfer = test_simple_read_write_transfer;
  bus->device = &bus->simple_read_write.base;

  bus->max_speed_hz = 50000000;

  rv = spi_bus_register(&bus->base, &bus_path[0]);
  rtems_test_assert(rv == 0);

  fd = open(&bus_path[0], O_RDWR);
  rtems_test_assert(fd >= 0);

  rv = ioctl(fd, SPI_BUS_OBTAIN);
  rtems_test_assert(rv == 0);

  rv = ioctl(fd, SPI_BUS_RELEASE);
  rtems_test_assert(rv == 0);

  rv = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  rtems_test_assert(rv == 0);
  rv = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &read_speed);
  rtems_test_assert(rv == 0);
  rtems_test_assert(read_speed == speed);

  speed = 60000000;
  rv = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  rtems_test_assert(rv == -1);

  rv = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb_first);
  rtems_test_assert(rv == 0);
  rv = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &read_lsb_first);
  rtems_test_assert(rv == 0);
  rtems_test_assert(read_lsb_first == lsb_first);

  rv = ioctl(fd, SPI_IOC_WR_MODE, &mode_8);
  rtems_test_assert(rv == 0);
  rv = ioctl(fd, SPI_IOC_RD_MODE, &read_mode_8);
  rtems_test_assert(rv == 0);
  rtems_test_assert(read_mode_8 == mode_8);

  rv = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
  rtems_test_assert(rv == 0);
  rv = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &read_bits_per_word);
  rtems_test_assert(rv == 0);
  rtems_test_assert(read_bits_per_word == bits_per_word);

  bits_per_word = 7;
  rv = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
  rtems_test_assert(rv == -1);

  rv = ioctl(fd, SPI_IOC_WR_MODE32, &mode_32);
  rtems_test_assert(rv == 0);
  rv = ioctl(fd, SPI_IOC_RD_MODE32, &read_mode_32);
  rtems_test_assert(rv == 0);
  rtems_test_assert(read_mode_32 == mode_32);

  bus->msg_count = 1;
  ioctl(fd, SPI_IOC_MESSAGE(8192), &msg);
  rtems_test_assert(bus->msg_count == 0);

  test_simple_read_write(bus, fd);

  rv = close(fd);
  rtems_test_assert(rv == 0);

  rv = unlink(&bus_path[0]);
  rtems_test_assert(rv == 0);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
}

static void Init(rtems_task_argument arg)
{
  (void)arg;

  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 2000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 7

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
