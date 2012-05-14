/*
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#ifndef _TOUCHSCREEN_H_
#define _TOUCHSCREEN_H_

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   * ioctl for lefthanded/righthanded mode.
   */

#define MW_UID_SET_RIGHTHANDED	0x4108
#define MW_UID_SET_LEFTHANDED	0x4109

  /*
   * touchscreen prototype entry points
   */

  rtems_device_driver touchscreen_initialize (rtems_device_major_number,
                                              rtems_device_minor_number,
                                              void *);

  rtems_device_driver touchscreen_open (rtems_device_major_number,
                                        rtems_device_minor_number, void *);

  rtems_device_driver touchscreen_control (rtems_device_major_number,
                                           rtems_device_minor_number, void *);

  rtems_device_driver touchscreen_close (rtems_device_major_number,
                                         rtems_device_minor_number, void *);

  rtems_device_driver touchscreen_read (rtems_device_major_number,
                                        rtems_device_minor_number, void *);

  rtems_device_driver touchscreen_write (rtems_device_major_number,
                                         rtems_device_minor_number, void *);

  rtems_device_driver touchscreen_control (rtems_device_major_number,
                                           rtems_device_minor_number, void *);

#define TOUCHSCREEN_DRIVER_TABLE_ENTRY \
  { touchscreen_initialize, touchscreen_open, touchscreen_close, \
    touchscreen_read, touchscreen_write, touchscreen_control }


#ifdef __cplusplus
}
#endif

#endif
