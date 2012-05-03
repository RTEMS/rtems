/*  gpio.c
 *
 *  GPIO driver for the Milkymist One board
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010, 2011 Sebastien Bourdeauducq
 *  COPYRIGHT (c) Yann Sionneau <yann.sionneau@telecom-sudparis.eu> (GSoC 2010)
 *  Telecom SudParis
 */

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <rtems.h>
#include <rtems/status-checks.h>
#include <bsp.h>
#include <rtems/libio.h>
#include "../include/system_conf.h"
#include "milkymist_gpio.h"

struct milkymist_gpio {
  char *name;
  unsigned int mask;
  bool readonly;
};

static const struct milkymist_gpio gpio[] = {
  {
    .name = "/dev/led1",
    .mask = GPIO_LED1,
    .readonly = false
  },
  {
    .name = "/dev/led2",
    .mask = GPIO_LED2,
    .readonly = false
  },
};

rtems_device_driver gpio_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  rtems_status_code sc;
  int i;

  for (i=0;i<sizeof(gpio)/sizeof(struct milkymist_gpio);i++) {
    sc = rtems_io_register_name(gpio[i].name, major, i);
    RTEMS_CHECK_SC(sc, "create GPIO device");
  }

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver gpio_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  unsigned int data;

  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;

  if (rw_args->offset > 0) {
    rw_args->bytes_moved = 0;
    return RTEMS_SUCCESSFUL;
  }

  rw_args->bytes_moved = 1;

  if (gpio[minor].readonly)
    data = MM_READ(MM_GPIO_IN);
  else
    data = MM_READ(MM_GPIO_OUT);

  if (data & gpio[minor].mask)
    *(uint8_t *)rw_args->buffer = '1';
  else
    *(uint8_t *)rw_args->buffer = '0';

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver gpio_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void *arg
)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *)arg;

  if (gpio[minor].readonly) {
    rw_args->bytes_moved = 0;
    return RTEMS_UNSATISFIED;
  }

  if (rw_args->offset > 0) {
    rw_args->bytes_moved = 0;
    return RTEMS_SUCCESSFUL;
  }

  rw_args->bytes_moved = 1;

  if (*(uint8_t *)rw_args->buffer == '1')
    MM_WRITE(MM_GPIO_OUT, MM_READ(MM_GPIO_OUT)|gpio[minor].mask);
  else
    MM_WRITE(MM_GPIO_OUT, MM_READ(MM_GPIO_OUT) & ~gpio[minor].mask);

  return RTEMS_SUCCESSFUL;
}
