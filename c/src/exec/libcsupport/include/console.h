/*  console.h
 *
 *  This file describes the Console Device Driver for all boards.
 *  This driver provides support for the standard C Library.
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

#ifndef _CONSOLE_DRIVER_h
#define _CONSOLE_DRIVER_h

#ifdef __cplusplus
extern "C" {
#endif

#define CONSOLE_DRIVER_TABLE_ENTRY \
  { console_initialize, console_open, console_close, \
    console_read, console_write, console_control }

rtems_device_driver console_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver console_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver console_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver console_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver console_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver console_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
