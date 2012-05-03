/*  milkymist_ir.h
 *
 *  Milkymist RC5 IR driver for RTEMS
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  COPYRIGHT (c) 2010 Sebastien Bourdeauducq
 */

#ifndef __MILKYMIST_IR_H_
#define __MILKYMIST_IR_H_

#ifdef __cplusplus
extern "C" {
#endif

rtems_device_driver ir_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver ir_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

rtems_device_driver ir_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

#define IR_DRIVER_TABLE_ENTRY {ir_initialize, \
ir_open, NULL, ir_read, NULL, NULL}

#ifdef __cplusplus
}
#endif

#endif /* __MILKYMIST_IR_H_ */
