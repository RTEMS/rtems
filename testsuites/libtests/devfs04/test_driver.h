/*  test_driver.h
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __TEST_DRIVER_h
#define __TEST_DRIVER_h

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_DRIVER_TABLE_ENTRY \
  { testDriver_initialize, testDriver_open, testDriver_close, testDriver_read, \
    testDriver_write, testDriver_control }

rtems_device_driver testDriver_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver testDriver_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver testDriver_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver testDriver_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver testDriver_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver testDriver_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
