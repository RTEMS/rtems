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

#include <rtems/rtems/status.h>

/*
 *
 *  The following defines the types for:
 *
 *    + major and minor numbers
 *    + the return type of a device driver entry
 *    + a pointer to a device driver entry
 *    + an entry in the the Device Driver Address Table.  Each entry in this
 *      table corresponds to an application provided device driver and
 *      defines the entry points for that device driver.
 */
 
typedef unsigned32 rtems_device_major_number;
typedef unsigned32 rtems_device_minor_number;
 
typedef rtems_status_code rtems_device_driver;
 
typedef rtems_device_driver ( *rtems_device_driver_entry )(
                 rtems_device_major_number,
                 rtems_device_minor_number,
                 void *
             );

typedef struct {
  rtems_device_driver_entry initialization; /* initialization procedure */
  rtems_device_driver_entry open;           /* open request procedure */
  rtems_device_driver_entry close;          /* close request procedure */
  rtems_device_driver_entry read;           /* read request procedure */
  rtems_device_driver_entry write;          /* write request procedure */
  rtems_device_driver_entry control;        /* special functions procedure */
}   rtems_driver_address_table;
 
/*
 * Table for the io device names
 */

typedef struct {
    char                     *device_name;
    unsigned32                device_name_length;
    rtems_device_major_number major;
    rtems_device_minor_number minor;
} rtems_driver_name_t;

/*
 *  This is the table of device names.
 */

/*
 *  The following declare the data required to manage the Driver
 *  Address Table and Device Name Table.
 */

SAPI_EXTERN unsigned32                  _IO_Number_of_drivers;
SAPI_EXTERN rtems_driver_address_table *_IO_Driver_address_table;
SAPI_EXTERN unsigned32                  _IO_Number_of_devices;
SAPI_EXTERN rtems_driver_name_t        *_IO_Driver_name_table;

/*
 *  _IO_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _IO_Manager_initialization(
  rtems_driver_address_table *driver_table,
  unsigned32                  number_of_drivers,
  unsigned32                  number_of_devices
);

/*
 *  rtems_io_register_name
 *
 *  DESCRIPTION:
 *
 *  Associate a name with a driver.
 *
 */

rtems_status_code rtems_io_register_name(
    char                      *device_name,
    rtems_device_major_number  major,
    rtems_device_minor_number  minor
);


/*
 *  rtems_io_lookup_name
 *
 *  DESCRIPTION:
 *
 *  Find what driver "owns" this name
 */

rtems_status_code rtems_io_lookup_name(
    const char           *name,
    rtems_driver_name_t **device_info
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
  void                      *argument
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
  void                      *argument
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
  void                      *argument
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
  void                      *argument
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
  void                      *argument
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
  void                      *argument
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

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
