#ifndef __paux_drv__
#define __paux_drv__
/***************************************************************************
 *
 * $Header$
 *
 * Copyright (c) 1999 ConnectTel, Inc. All Rights Reserved.
 *
 * MODULE DESCRIPTION: Prototype routines for the paux driver.
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
 * Revision 1.1  2000/08/30 08:15:30  joel
 * 2000-08-26  Rosimildo da Silva  <rdasilva@connecttel.com>
 *
 * 	* Major rework of the "/dev/console" driver.
 * 	* Added termios support for stdin ( keyboard ).
 * 	* Added ioctls() to support modes similar to Linux( XLATE,
 * 	RAW, MEDIUMRAW ).
 * 	* Added Keyboard mapping and handling of the keyboard's leds.
 * 	* Added Micro FrameBuffer driver ( "/dev/fb0" ) for bare VGA
 * 	controller ( 16 colors ).
 * 	* Added PS/2 and Serial mouse support for PC386 BSP.
 * 	* console/defkeymap.c: New file.
 * 	* console/fb_vga.c: New file.
 * 	* console/fb_vga.h: New file.
 * 	* console/i386kbd.h: New file.
 * 	* console/kd.h: New file.
 * 	* console/keyboard.c: New file.
 * 	* console/keyboard.h: New file.
 * 	* console/mouse_parser.c: New file.
 * 	* console/mouse_parser.h: New file.
 * 	* console/pc_keyb.c: New file.
 * 	* console/ps2_drv.h: New file.
 * 	* console/ps2_mouse.c: New file.
 * 	* console/ps2_mouse.h: New file.
 * 	* console/serial_mouse.c: New file.
 * 	* console/serial_mouse.h: New file.
 * 	* console/vgainit.c: New file.
 * 	* console/vt.c: New file.
 * 	* console/Makefile.am: Reflect new files.
 * 	* console/console.c, console/inch.c, console/outch.c: Console
 * 	functionality modifications.
 * 	* startup/Makefile.am: Pick up tty_drv.c and gdb_glue.c
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
/* end of include file */

#endif  /* __paux_drv__  */
