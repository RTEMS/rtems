/**
 * @file
 *
 * @ingroup ClassicIO
 *
 * @brief Classic Input/Output Manager API.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_IO_H
#define _RTEMS_IO_H

#ifndef SAPI_IO_EXTERN
#define SAPI_IO_EXTERN extern
#endif

#include <rtems/rtems/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicIO Input/Output
 *
 * @ingroup ClassicRTEMS
 *
 * @{
 */

typedef uint32_t rtems_device_major_number;

typedef uint32_t rtems_device_minor_number;

typedef rtems_status_code rtems_device_driver;

typedef rtems_device_driver (*rtems_device_driver_entry)(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

typedef struct {
  rtems_device_driver_entry initialization_entry;
  rtems_device_driver_entry open_entry;
  rtems_device_driver_entry close_entry;
  rtems_device_driver_entry read_entry;
  rtems_device_driver_entry write_entry;
  rtems_device_driver_entry control_entry;
} rtems_driver_address_table;

/**
 * @name Device Driver Maintainance
 *
 * @{
 */

/**
 * @brief Returns @c RTEMS_IO_ERROR.
 *
 * @retval RTEMS_IO_ERROR Only this one.
 */
rtems_status_code rtems_io_driver_io_error(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
);

/**
 * @brief Registers and initializes the device with the device driver table
 * @a driver_table and major number @a major.
 *
 * If the major number equals zero a major number will be obtained.  The major
 * number of the registered driver will be returned in @a registered_major.
 *
 * After a successful registration rtems_io_initialize() will be called to
 * initialize the device.
 *
 * @retval RTEMS_SUCCESSFUL Device successfully registered and initialized.
 * @retval RTEMS_INVALID_ADDRESS Pointer to driver table or to registered
 * major number are invalid.  Device driver table is empty.
 * @retval RTEMS_INVALID_NUMBER Invalid major number.
 * @retval RTEMS_TOO_MANY No major number available.
 * @retval RTEMS_RESOURCE_IN_USE Major number in use.
 * @retval RTEMS_CALLED_FROM_ISR Called from interrupt context.
 * @retval * Status code depends on rtems_io_initialize().
 */
rtems_status_code rtems_io_register_driver(
  rtems_device_major_number major,
  const rtems_driver_address_table *driver_table,
  rtems_device_major_number *registered_major
);

/**
 * @brief Unregisters the device driver with number @a major.
 *
 * @retval RTEMS_SUCCESSFUL Device driver successfully unregistered.
 * @retval RTEMS_UNSATISFIED Invalid major number.
 * @retval RTEMS_CALLED_FROM_ISR Called from interrupt context.
 */
rtems_status_code rtems_io_unregister_driver(
  rtems_device_major_number major
);

/**
 * @brief Registers the name @a device_name in the file system for the device
 * with number tuple @a major and @a minor.
 *
 * @retval RTEMS_SUCCESSFUL Name successfully registered.
 * @retval RTEMS_TOO_MANY Name already in use or other errors.
 */
rtems_status_code rtems_io_register_name(
  const char *device_name,
  rtems_device_major_number major,
  rtems_device_minor_number minor
);

/** @} */

/**
 * @name Device Driver Invocation
 *
 * @{
 */

rtems_status_code rtems_io_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
);

rtems_status_code rtems_io_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
);

rtems_status_code rtems_io_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
);

rtems_status_code rtems_io_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
);

rtems_status_code rtems_io_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
);

rtems_status_code rtems_io_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
);

/** @} */

/** @} */

typedef struct {
    const char               *device_name;
    size_t                    device_name_length;
    rtems_device_major_number major;
    rtems_device_minor_number minor;
} rtems_driver_name_t;

/**
 * @deprecated Use stat() instead.
 */
rtems_status_code rtems_io_lookup_name(
    const char           *name,
    rtems_driver_name_t  *device_info
) RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;

SAPI_IO_EXTERN uint32_t _IO_Number_of_drivers;

SAPI_IO_EXTERN rtems_driver_address_table *_IO_Driver_address_table;

void _IO_Manager_initialization( void );

void _IO_Initialize_all_drivers( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
