#ifndef __tty_drv__
#define __tty_drv__
/***************************************************************************
 *
 * $Header$
 *
 * Copyright (c) 1999 ConnectTel, Inc. All Rights Reserved.
 *
 * MODULE DESCRIPTION: Prototype routines for the ttySx driver.
 *
 * by: Rosimildo da Silva:
 *     rdasilva@connecttel.com
 *     http://www.connecttel.com
 *
 * MODIFICATION/HISTORY:
 *
 * $Log$
 * Revision 1.3  2004/04/21 10:42:44  ralf
 * Remove stray white spaces.
 *
 * Revision 1.2  2004/04/15 13:26:12  ralf
 * Remove stray white spaces.
 *
 * Revision 1.1  2000/08/30 08:18:56  joel
 * 2000-08-26  Rosimildo da Silva  <rdasilva@connecttel.com>
 *
 * 	* shared/comm: Added "/dev/ttyS1" & "/dev/ttyS2" support for
 * 	the i386 BSPs.
 * 	* shared/comm/gdb_glue.c: New file.
 * 	* shared/comm/i386_io.c: New file.
 * 	* shared/comm/tty_drv.c: New file.
 * 	* shared/comm/tty_drv.h: New file.
 * 	* shared/comm/Makefile.am: Account for new files.
 * 	* shared/comm/uart.c: Adds support for sending characters to
 * 	another "line discipline."
 *
 ****************************************************************************/

/* functions */
#ifdef __cplusplus
extern "C" {
#endif

/* ttyS1 entry points */
rtems_device_driver tty1_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver tty1_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver tty1_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

/* tty1 & tty2 shared entry points */
rtems_device_driver tty_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver tty_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver tty_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

/* tty2 entry points */
rtems_device_driver tty2_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver tty2_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver tty2_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#define TTY1_DRIVER_TABLE_ENTRY \
  { tty1_initialize, tty1_open, tty_close, \
    tty_read, tty_write, tty1_control }

#define TTY2_DRIVER_TABLE_ENTRY \
  { tty2_initialize, tty2_open, tty_close, \
    tty_read, tty_write, tty2_control }

#ifdef __cplusplus
}
#endif
/* end of include file */

#endif  /* __tty_drv__  */
