/*  sio.h
 *
 *  sio device driver for UART SCC and SMC
 *
 *  COPYRIGHT (c) 1997 Pacific Computing
 * 
 *  $Id$
 */

#ifndef _SIO_DRIVER_h
#define _SIO_DRIVER_h

#ifdef __cplusplus
extern "C" {
#endif

#define SIO_DRIVER_TABLE_ENTRY \
  { sio_initialize, sio_open, sio_close, \
    sio_read, sio_write, sio_control }

rtems_device_driver sio_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver sio_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver sio_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver sio_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver sio_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver sio_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
