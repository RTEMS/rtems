#ifndef __fbvga_drv__
#define __fbvga_drv__
/***************************************************************************
 *
 * $Header$
 *
 * Copyright (c) 2000  -- Rosimildo da Silva.
 *  
 * MODULE DESCRIPTION: 
 * Prototype routines for the fbvga driver.
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

/* fbvga prototype entry points */
rtems_device_driver fbvga_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver fbvga_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver fbvga_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver fbvga_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);


rtems_device_driver fbvga_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver fbvga_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver fbvga_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#define FBVGA_DRIVER_TABLE_ENTRY \
  { fbvga_initialize, fbvga_open, fbvga_close, \
    fbvga_read, fbvga_write, fbvga_control }

#ifdef __cplusplus
}
#endif
/* end of include file */

#endif  /* __fbvga_drv__  */


