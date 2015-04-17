/**
 * @file
 *
 * @brief Classic Input/Output Manager API
 * 
 * This file emulates the old Classic RTEMS IO manager directives
 * which register and lookup names using the in-memory filesystem.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
 */
/**@{**/

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
 */
/**@{**/

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
 * @brief Unregister a driver from the device driver table.
 *
 * @param[in] major is the device major number.
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
 * This assumes that all registered devices are character devices.
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
 * @brief IO driver initialization.
 *
 * This routine is the initialization directive of the IO manager.
 *
 * @param[in] major is the device drive number
 * @param[in] minor is the device number
 * @param[in] argument is the pointer to the argument(s)
 *
 * @return status code
 */
rtems_status_code rtems_io_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
);

/**
 * @brief Opening for the IO manager.
 *  
 * Opens a device driver with the number @a major.
 *
 * @param[in] major is the device driver number.
 * @param[in] minor is the device number.
 * @param[in] argument is the pointer to the argument(s).
 *
 * @return Status code.
 */
rtems_status_code rtems_io_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
);

/**
 * @brief Closing for the IO manager.
 *  
 * This routine is the close directive of the IO manager.
 *
 * @param[in] major is the device driver number.
 * @param[in] minor is the device number.
 * @param[in] argument is the pointer to the argument(s).
 *
 * @return Status code.
 */
rtems_status_code rtems_io_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
);

/**
 * @brief Reading for the IO manager.
 *  
 * This routine is the read directive of the IO manager.
 *
 * @param[in] major is the device driver number.
 * @param[in] minor is the device number.
 * @param[in] argument is the pointer to the argument(s).
 *
 * @return Status code.
 */
rtems_status_code rtems_io_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
);

/**
 * @brief Writing for the IO manager.
 *  
 * This routine is the write directive of the IO manager.
 *
 * @param[in] major is the device driver number.
 * @param[in] minor is the device number.
 * @param[in] argument is the pointer to the argument(s).
 *
 * @return Status code.
 */
rtems_status_code rtems_io_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *argument
);

/**
 * @brief Control for the IO manager.
 *  
 * This routine is the control directive of the IO manager.
 *
 * @param[in] major is the device driver number.
 * @param[in] minor is the device number.
 * @param[in] argument is the pointer to the argument(s).
 *
 * @return Status code.
 */
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

extern const size_t _IO_Number_of_drivers;

extern rtems_driver_address_table _IO_Driver_address_table[];

extern bool _IO_All_drivers_initialized;

/**
 * @brief Initialization of all device drivers.
 *
 * Initializes all device drivers.
 */
void _IO_Initialize_all_drivers( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
