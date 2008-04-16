/*
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 *
 * $Id$
 */

#ifndef _FB_H_
#define _FB_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* fbds prototype entry points */
  rtems_device_driver fbds_initialize (rtems_device_major_number,
                                       rtems_device_minor_number, void *);

  rtems_device_driver fbds_open (rtems_device_major_number,
                                 rtems_device_minor_number, void *);

  rtems_device_driver fbds_control (rtems_device_major_number,
                                    rtems_device_minor_number, void *);

  rtems_device_driver fbds_close (rtems_device_major_number,
                                  rtems_device_minor_number, void *);

  rtems_device_driver fbds_read (rtems_device_major_number,
                                 rtems_device_minor_number, void *);

  rtems_device_driver fbds_write (rtems_device_major_number,
                                  rtems_device_minor_number, void *);

  rtems_device_driver fbds_control (rtems_device_major_number,
                                    rtems_device_minor_number, void *);

#define FB_DRIVER_TABLE_ENTRY \
  { fbds_initialize, fbds_open, fbds_close, \
    fbds_read, fbds_write, fbds_control }


#ifdef __cplusplus
}
#endif

#endif
