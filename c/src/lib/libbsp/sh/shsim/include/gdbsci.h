/*
 *  Driver for gdb's sh-simulator's sci emulation
 *
 *  Author: Ralf Corsepius (corsepiu@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 2001, Ralf Corsepius, Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *  $Id$
 */

#ifndef _sh_gdbsci_h
#define _sh_gdbsci_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 */ 

#define DEVGDBSCI_DRIVER_TABLE_ENTRY \
  { sh_gdbsci_initialize, sh_gdbsci_open, sh_gdbsci_close, sh_gdbsci_read, \
      sh_gdbsci_write, sh_gdbsci_control }

extern rtems_device_driver sh_gdbsci_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

extern rtems_device_driver sh_gdbsci_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

extern rtems_device_driver sh_gdbsci_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

extern rtems_device_driver sh_gdbsci_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

extern rtems_device_driver sh_gdbsci_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

extern rtems_device_driver sh_gdbsci_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

#ifdef __cplusplus
}
#endif

#endif
