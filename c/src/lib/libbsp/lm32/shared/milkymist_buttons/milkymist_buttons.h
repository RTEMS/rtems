/*  milkymist_buttons.h
 *
 *  Milkymist buttons driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2011 Sebastien Bourdeauducq
 */

#ifndef __MILKYMIST_BUTTONS_H_
#define __MILKYMIST_BUTTONS_H_

#ifdef __cplusplus
extern "C" {
#endif

rtems_device_driver buttons_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver buttons_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver buttons_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define BUTTONS_DRIVER_TABLE_ENTRY { buttons_initialize, \
buttons_open, NULL, buttons_read, NULL, NULL}

#ifdef __cplusplus
}
#endif

#endif /* __MILKYMIST_BUTTONS_H_ */
