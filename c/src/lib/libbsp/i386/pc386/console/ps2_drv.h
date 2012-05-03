#ifndef __paux_drv__
#define __paux_drv__
/***************************************************************************
 *
 * Copyright (c) 1999 ConnectTel, Inc. All Rights Reserved.
 *
 * MODULE DESCRIPTION: Prototype routines for the paux driver.
 *
 * by: Rosimildo da Silva:
 *     rdasilva@connecttel.com
 *     http://www.connecttel.com
 *
 ****************************************************************************/

/* functions */
#ifdef __cplusplus
extern "C" {
#endif

/* paux prototype entry points */
rtems_device_driver paux_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver paux_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver paux_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver paux_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver paux_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver paux_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver paux_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#define PAUX_DRIVER_TABLE_ENTRY \
  { paux_initialize, paux_open, paux_close, \
    paux_read, paux_write, paux_control }

#ifdef __cplusplus
}
#endif

#endif  /* __paux_drv__  */
