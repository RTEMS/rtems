/*  io.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Input/Output Manager.  This manager provides a well defined
 *  mechanism for accessing device drivers and a structured methodology for
 *  organizing device drivers.
 *
 *  Directives provided are:
 *
 *     + initialize a device driver
 *     + open a device driver
 *     + close a device driver
 *     + read from a device driver
 *     + write to a device driver
 *     + special device services
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_IO_h
#define __RTEMS_IO_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/config.h>

/*
 *  The following type defines the set of IO operations which are
 *  recognized by _IO_Handler and can be supported by a RTEMS
 *  device driver.
 */

typedef enum {
  IO_INITIALIZE_OPERATION = 0,
  IO_OPEN_OPERATION       = 1,
  IO_CLOSE_OPERATION      = 2,
  IO_READ_OPERATION       = 3,
  IO_WRITE_OPERATION      = 4,
  IO_CONTROL_OPERATION    = 5
}  IO_operations;

/*
 *  The following declare the data required to manage the Device Driver
 *  Address Table.
 */

EXTERN unsigned32                          _IO_Number_of_drivers;
EXTERN rtems_driver_address_table *_IO_Driver_address_table;

/*
 *  _IO_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

STATIC INLINE void _IO_Manager_initialization(
  rtems_driver_address_table *driver_table,
  unsigned32                          number_of_drivers
);

/*
 *  rtems_io_initialize
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_io_initialize directive.  It is invoked
 *  to initialize a device driver or an individual device.
 */

rtems_status_code rtems_io_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
);

/*
 *  rtems_io_open
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_io_open directive.  It is invoked
 *  to open a device.
 */

rtems_status_code rtems_io_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
);

/*
 *  rtems_io_close
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_io_close directive.  It is invoked
 *  to close a device.
 */

rtems_status_code rtems_io_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
);

/*
 *  rtems_io_read
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_io_read directive.  It is invoked
 *  to read from a device.
 */

rtems_status_code rtems_io_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
);

/*
 *  rtems_io_write
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_io_write directive.  It is invoked
 *  to write to a device.
 */

rtems_status_code rtems_io_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
);

/*
 *  rtems_io_control
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_io_control directive.  It is invoked
 *  to perform a device specific operation on a device.
 */

rtems_status_code rtems_io_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void             *argument,
  unsigned32       *return_value
);

/*
 *  _IO_Initialize_all_drivers
 *
 *  DESCRIPTION:
 *
 *  This routine initializes all of the device drivers configured
 *  in the Device Driver Address Table.
 */

void _IO_Initialize_all_drivers( void );

/*
 *  _IO_Handler_routine
 *
 *  DESCRIPTION:
 *
 *  This routine provides the common foundation for all of the IO
 *  Manager's directives.
 */

rtems_status_code _IO_Handler_routine(
  IO_operations              operation,
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument,
  unsigned32                *return_value
);

#include <rtems/io.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
