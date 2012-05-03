/*  milkymist_gpio.h
 *
 *  Milkymist GPIO driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 */

#ifndef __MILKYMIST_GPIO_H_
#define __MILKYMIST_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

rtems_device_driver gpio_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver gpio_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver gpio_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

#define GPIO_DRIVER_TABLE_ENTRY { gpio_initialize, \
NULL, NULL, gpio_read, gpio_write, NULL}

#ifdef __cplusplus
}
#endif

#endif /* __MILKYMIST_GPIO_H_ */
