/*  stubdrv.h
 *
 *  This file describes the Stub Driver for all boards.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __STUB_DRIVER_h
#define __STUB_DRIVER_h

#ifdef __cplusplus
extern "C" {
#endif

#define STUB_DRIVER_TABLE_ENTRY \
  { Stub_initialize, NULL, NULL, NULL, NULL, NULL }

#define STUB_SUCCESSFUL RTEMS_SUCCESSFUL

rtems_device_driver Stub_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver Stub_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver Stub_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver Stub_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver Stub_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver Stub_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
