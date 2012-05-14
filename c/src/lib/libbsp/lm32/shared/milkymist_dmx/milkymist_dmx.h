/*  milkymist_dmx.h
 *
 *  Milkymist DMX512 driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 */

#ifndef __MILKYMIST_DMX_H_
#define __MILKYMIST_DMX_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DMX_SET_THRU       0x4400
#define DMX_GET_THRU       0x4401

rtems_device_driver dmx_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver dmx_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver dmx_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver dmx_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define DMX_DRIVER_TABLE_ENTRY {dmx_initialize, \
NULL, NULL, dmx_read, dmx_write, dmx_control}

#ifdef __cplusplus
}
#endif

#endif /* __MILKYMIST_DMX_H_ */
