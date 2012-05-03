/*  ipl.h
 *
 *  IPL console driver
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  Based on work:
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __IPL_DRIVER_h
#define __IPL_DRIVER_h

#ifdef __cplusplus
extern "C" {
#endif

#define IPL_DRIVER_TABLE_ENTRY \
  { ipl_console_initialize, ipl_console_open, ipl_console_close, \
    ipl_console_read, ipl_console_write, ipl_console_control }


#define NULL_SUCCESSFUL RTEMS_SUCCESSFUL

rtems_device_driver ipl_console_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver ipl_console_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver ipl_console_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver ipl_console_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver ipl_console_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver ipl_console_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
