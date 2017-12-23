#ifdef ppc405
#ifndef __tty_drv__
#define __tty_drv__

/* functions */
#ifdef __cplusplus
extern "C" {
#endif


/* ttyS1 entry points */
rtems_device_driver tty0_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver tty0_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver tty0_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);


/* tty1 & tty2 shared entry points */
rtems_device_driver tty0_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);


rtems_device_driver tty0_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver tty0_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);


#define TTY0_DRIVER_TABLE_ENTRY \
  { tty0_initialize, tty0_open, tty0_close, \
    tty0_read, tty0_write, tty0_control }


#ifdef __cplusplus
}
#endif
/* end of include file */

#endif  /* __tty_drv__  */
#endif /* ppc405 */
