#ifndef __serial_mouse_drv__
#define __serial_mouse_drv__
/***************************************************************************
 *
 * $Header$
 *
 * Copyright (c) 1999 ConnectTel, Inc. All Rights Reserved.
 *  
 * MODULE DESCRIPTION: Prototype routines for the /dev/mouse driver.
 *
 * by: Rosimildo da Silva:
 *     rdasilva@connecttel.com
 *     http://www.connecttel.com
 *
 * MODIFICATION/HISTORY:
 *
 * $Log$
 ****************************************************************************/

/* functions */
#ifdef __cplusplus
extern "C" {
#endif


/* ttyS1 entry points */
rtems_device_driver serial_mouse_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver serial_mouse_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver serial_mouse_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);


/* serial_mouse  entry points */
rtems_device_driver serial_mouse_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);


rtems_device_driver serial_mouse_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver serial_mouse_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);


/* Select the mouse type:  "ms","pc","ps2" */
#define  MOUSE_TYPE        "ms"  

/* Select the serial port for the serial mouse driver */
#define  SERIAL_MOUSE_COM1  1
/* #define  SERIAL_MOUSE_COM2  1  */


#define SERIAL_MOUSE_DRIVER_TABLE_ENTRY \
  { serial_mouse_initialize, serial_mouse_open, serial_mouse_close, \
    serial_mouse_read, serial_mouse_write, serial_mouse_control }

#ifdef __cplusplus
}
#endif
/* end of include file */

#endif  /* __tty_drv__  */


