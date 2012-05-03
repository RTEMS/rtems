/*
 * RTEMS for Nintendo DS touchscreen driver.
 *
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include <bsp.h>
#include <rtems/irq.h>
#include <rtems/libio.h>
#include <nds.h>

#include <rtems/mw_uid.h>
#include "touchscreen.h"

/*
 * from parser.c
 */

void register_mou_msg_queue (char *q_name);
void unregister_mou_msg_queue (void);
void touchscreen_sethand (int);

/*
 * touchscreen device driver initialize entry point.
 */

rtems_device_driver
touchscreen_initialize (rtems_device_major_number major,
                        rtems_device_minor_number minor, void *arg)
{
  rtems_status_code status;

  printk ("[+] touchscreen started\n");

  touchscreen_sethand (1);

  /* register the device */
  status = rtems_io_register_name ("/dev/mouse", major, 0);
  if (status != RTEMS_SUCCESSFUL) {
    printk ("[!] error registering touchscreen\n");
    rtems_fatal_error_occurred (status);
  }
  return RTEMS_SUCCESSFUL;
}

/*
 * touchscreen device driver open operation.
 */

rtems_device_driver
touchscreen_open (rtems_device_major_number major,
                  rtems_device_minor_number minor, void *arg)
{
  return RTEMS_SUCCESSFUL;
}

/*
 * touchscreen device driver close operation.
 */

rtems_device_driver
touchscreen_close (rtems_device_major_number major,
                   rtems_device_minor_number minor, void *arg)
{
  return RTEMS_SUCCESSFUL;
}

/*
 * touchscreen device driver read operation.
 */

rtems_device_driver
touchscreen_read (rtems_device_major_number major,
                  rtems_device_minor_number minor, void *arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;
  rw_args->bytes_moved = 0;

  return RTEMS_SUCCESSFUL;
}

/*
 * touchscreen device driver write operation.
 */

rtems_device_driver
touchscreen_write (rtems_device_major_number major,
                   rtems_device_minor_number minor, void *arg)
{
  rtems_libio_rw_args_t *rw_args = (rtems_libio_rw_args_t *) arg;
  rw_args->bytes_moved = 0;

  return RTEMS_SUCCESSFUL;
}

/*
 * ioctl entry point.
 */

rtems_device_driver
touchscreen_control (rtems_device_major_number major,
                     rtems_device_minor_number minor, void *arg)
{
  rtems_libio_ioctl_args_t *args = arg;

  switch (args->command) {
  case MW_UID_REGISTER_DEVICE:
    register_mou_msg_queue (args->buffer);
    break;
  case MW_UID_UNREGISTER_DEVICE:
    unregister_mou_msg_queue ();
    break;
  case MW_UID_SET_LEFTHANDED:
    touchscreen_sethand (0);
    break;
  case MW_UID_SET_RIGHTHANDED:
    touchscreen_sethand (1);
    break;
  }
  args->ioctl_return = 0;

  return RTEMS_SUCCESSFUL;
}
