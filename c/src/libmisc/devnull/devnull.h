/*  devnull.h
 *
 *  Null device driver, derived from rtems' stub driver.
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __NULL_DRIVER_h
#define __NULL_DRIVER_h

#ifdef __cplusplus
extern "C" {
#endif

#define DEVNULL_DRIVER_TABLE_ENTRY \
  { null_initialize, null_open, null_close, null_read, \
    null_write, null_control }

#define NULL_SUCCESSFUL RTEMS_SUCCESSFUL

rtems_device_driver null_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver null_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver null_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver null_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver null_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver null_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
