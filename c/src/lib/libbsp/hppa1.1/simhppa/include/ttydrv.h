/*
 *  $Id$
 */

/*
 * definitions for the tty driver
 */

#ifndef _RTEMS_TTYDRV_H
#define _RTEMS_TTYDRV_H

rtems_device_driver
tty_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  );

rtems_device_driver
tty_open(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  );

rtems_device_driver
tty_close(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  );

rtems_device_driver
tty_control(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  );

rtems_device_driver
tty_read(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  );

rtems_device_driver
tty_write(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  );

#endif /* _RTEMS_TTYDRV_H */
