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
