/*  ramdisk.h
 *
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RAMDISK_DRIVER_h
#define _RAMDISK_DRIVER_h

#ifdef __cplusplus
extern "C" {
#endif


rtems_device_driver ramdisk_initialize(rtems_device_major_number major,
                                       rtems_device_minor_number minor,
                                       void *arg);

rtems_device_driver ramdisk_open(rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void *arg);

rtems_device_driver ramdisk_close(rtems_device_major_number major,
                                  rtems_device_minor_number minor,
                                  void *arg);

rtems_device_driver ramdisk_read(rtems_device_major_number major,
                                 rtems_device_minor_number minor,
                                 void *arg);

rtems_device_driver ramdisk_write(rtems_device_major_number major,
                                  rtems_device_minor_number minor,
                                  void *arg);

rtems_device_driver ramdisk_control(rtems_device_major_number major,
                                    rtems_device_minor_number minor,
                                    void *arg);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
