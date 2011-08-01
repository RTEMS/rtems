/*  milkymist_usbinput.h
 *
 *  Milkymist USB input devices driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 */

#ifndef __MILKYMIST_USBINPUT_H_
#define __MILKYMIST_USBINPUT_H_

#ifdef __cplusplus
extern "C" {
#endif

rtems_device_driver usbinput_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver usbinput_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver usbinput_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define USBINPUT_DRIVER_TABLE_ENTRY {usbinput_initialize, \
usbinput_open, NULL, usbinput_read, NULL, NULL}

#ifdef __cplusplus
}
#endif

#endif /* __MILKYMIST_USBINPUT_H_ */
