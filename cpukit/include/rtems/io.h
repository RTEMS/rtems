/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief This header file defines the IO Manager API.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/io/if/header */

#ifndef _RTEMS_IO_H
#define _RTEMS_IO_H

#include <stdint.h>
#include <rtems/rtems/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/io/if/group */

/**
 * @defgroup RTEMSAPIClassicIO I/O Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Input/Output (I/O) Manager provides a well-defined mechanism for
 *   accessing device drivers and a structured methodology for organizing
 *   device drivers.
 */

/* Generated from spec:/rtems/io/if/device-driver */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief This type shall be used in device driver entry declarations and
 *   definitions.
 *
 * @par Notes
 * Device driver entries return an #rtems_status_code status code. This type
 * definition helps to document device driver entries in the source code.
 */
typedef rtems_status_code rtems_device_driver;

/* Generated from spec:/rtems/io/if/device-major-number */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief This integer type represents the major number of devices.
 *
 * @par Notes
 * The major number of a device is determined by rtems_io_register_driver() and
 * the application configuration (see #CONFIGURE_MAXIMUM_DRIVERS) .
 */
typedef uint32_t rtems_device_major_number;

/* Generated from spec:/rtems/io/if/device-minor-number */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief This integer type represents the minor number of devices.
 *
 * @par Notes
 * The minor number of devices is managed by the device driver.
 */
typedef uint32_t rtems_device_minor_number;

/* Generated from spec:/rtems/io/if/device-driver-entry */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief Device driver entries shall have this type.
 */
typedef rtems_device_driver ( *rtems_device_driver_entry )(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

/* Generated from spec:/rtems/io/if/driver-address-table */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief This structure contains the device driver entries.
 *
 * This structure is used to register a device driver via
 * rtems_io_register_driver().
 */
typedef struct {
  /**
   * @brief This member is the device driver initialization entry.
   *
   * This entry is called by rtems_io_initialize().
   */
  rtems_device_driver_entry initialization_entry;

  /**
   * @brief This member is the device driver open entry.
   *
   * This entry is called by rtems_io_open().
   */
  rtems_device_driver_entry open_entry;

  /**
   * @brief This member is the device driver close entry.
   *
   * This entry is called by rtems_io_close().
   */
  rtems_device_driver_entry close_entry;

  /**
   * @brief This member is the device driver read entry.
   *
   * This entry is called by rtems_io_read().
   */
  rtems_device_driver_entry read_entry;

  /**
   * @brief This member is the device driver write entry.
   *
   * This entry is called by rtems_io_write().
   */
  rtems_device_driver_entry write_entry;

  /**
   * @brief This member is the device driver control entry.
   *
   * This entry is called by rtems_io_control().
   */
  rtems_device_driver_entry control_entry;
} rtems_driver_address_table;

/* Generated from spec:/rtems/io/if/register-driver */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief Registers and initializes the device with the specified device driver
 *   address table and device major number in the Device Driver Table.
 *
 * @param major is the device major number.  Use a value of zero to let the
 *   system obtain a device major number automatically.
 *
 * @param driver_table is the device driver address table.
 *
 * @param[out] registered_major is the pointer to a device major number
 *   variable.  When the directive call is successful, the device major number
 *   of the registered device will be stored in this variable.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The device major number of the device was
 *   NULL.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The device driver address table was empty.
 *
 * @retval ::RTEMS_INVALID_NUMBER The device major number of the device was out
 *   of range, see #CONFIGURE_MAXIMUM_DRIVERS.
 *
 * @retval ::RTEMS_TOO_MANY The system was unable to obtain a device major
 *   number.
 *
 * @retval ::RTEMS_RESOURCE_IN_USE The device major number was already in use.
 *
 * @retval ::RTEMS_CALLED_FROM_ISR The directive was called from interrupt
 *   context.
 *
 * @return Other status codes may be returned by rtems_io_initialize().
 *
 * @par Notes
 * @parblock
 * If the device major number equals zero a device major number will be
 * obtained.  The device major number of the registered driver will be
 * returned.
 *
 * After a successful registration, the rtems_io_initialize() directive will be
 * called to initialize the device.
 * @endparblock
 */
rtems_status_code rtems_io_register_driver(
  rtems_device_major_number         major,
  const rtems_driver_address_table *driver_table,
  rtems_device_major_number        *registered_major
);

/* Generated from spec:/rtems/io/if/unregister-driver */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief Removes a device driver specified by the device major number from the
 *   Device Driver Table.
 *
 * @param major is the major number of the device.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_UNSATISFIED The device major number was invalid.
 *
 * @retval ::RTEMS_CALLED_FROM_ISR The directive was called from interrupt
 *   context.
 *
 * @par Notes
 * Currently no specific checks are made and the driver is not closed.
 */
rtems_status_code rtems_io_unregister_driver(
  rtems_device_major_number major
);

/* Generated from spec:/rtems/io/if/initialize */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief Initializes the device specified by the device major and minor
 *   numbers.
 *
 * @param major is the major number of the device.
 *
 * @param minor is the minor number of the device.
 *
 * @param argument is the argument passed to the device driver initialization
 *   entry.
 *
 * This directive calls the device driver initialization entry registered in
 * the Device Driver Table for the specified device major number.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NUMBER The device major number was invalid.
 *
 * @return Other status codes may be returned by the device driver
 *   initialization entry.
 *
 * @par Notes
 * @parblock
 * This directive is automatically invoked for each device driver defined by
 * the application configuration during the system initialization and via the
 * rtems_io_register_driver() directive.
 *
 * A device driver initialization entry is responsible for initializing all
 * hardware and data structures associated with a device.  If necessary, it can
 * allocate memory to be used during other operations.
 * @endparblock
 */
rtems_status_code rtems_io_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *argument
);

/* Generated from spec:/rtems/io/if/register-name */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief Registers the device specified by the device major and minor numbers
 *   in the file system under the specified name.
 *
 * @param device_name is the device name in the file system.
 *
 * @param major is the device major number.
 *
 * @param minor is the device minor number.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_TOO_MANY The name was already in use or other errors
 *   occurred.
 *
 * @par Notes
 * The device is registered as a character device.
 */
rtems_status_code rtems_io_register_name(
  const char               *device_name,
  rtems_device_major_number major,
  rtems_device_minor_number minor
);

/* Generated from spec:/rtems/io/if/open */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief Opens the device specified by the device major and minor numbers.
 *
 * @param major is the major number of the device.
 *
 * @param minor is the minor number of the device.
 *
 * @param argument is the argument passed to the device driver close entry.
 *
 * This directive calls the device driver open entry registered in the Device
 * Driver Table for the specified device major number.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NUMBER The device major number was invalid.
 *
 * @return Other status codes may be returned by the device driver open entry.
 *
 * @par Notes
 * The open entry point is commonly used by device drivers to provide exclusive
 * access to a device.
 */
rtems_status_code rtems_io_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *argument
);

/* Generated from spec:/rtems/io/if/close */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief Closes the device specified by the device major and minor numbers.
 *
 * @param major is the major number of the device.
 *
 * @param minor is the minor number of the device.
 *
 * @param argument is the argument passed to the device driver close entry.
 *
 * This directive calls the device driver close entry registered in the Device
 * Driver Table for the specified device major number.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NUMBER The device major number was invalid.
 *
 * @return Other status codes may be returned by the device driver close entry.
 *
 * @par Notes
 * The close entry point is commonly used by device drivers to relinquish
 * exclusive access to a device.
 */
rtems_status_code rtems_io_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *argument
);

/* Generated from spec:/rtems/io/if/read */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief Reads from the device specified by the device major and minor
 *   numbers.
 *
 * @param major is the major number of the device.
 *
 * @param minor is the minor number of the device.
 *
 * @param argument is the argument passed to the device driver read entry.
 *
 * This directive calls the device driver read entry registered in the Device
 * Driver Table for the specified device major number.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NUMBER The device major number was invalid.
 *
 * @return Other status codes may be returned by the device driver read entry.
 *
 * @par Notes
 * Read operations typically require a buffer address as part of the argument
 * parameter block.  The contents of this buffer will be replaced with data
 * from the device.
 */
rtems_status_code rtems_io_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *argument
);

/* Generated from spec:/rtems/io/if/write */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief Writes to the device specified by the device major and minor numbers.
 *
 * @param major is the major number of the device.
 *
 * @param minor is the minor number of the device.
 *
 * @param argument is the argument passed to the device driver write entry.
 *
 * This directive calls the device driver write entry registered in the Device
 * Driver Table for the specified device major number.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NUMBER The device major number was invalid.
 *
 * @return Other status codes may be returned by the device driver write entry.
 *
 * @par Notes
 * Write operations typically require a buffer address as part of the argument
 * parameter block.  The contents of this buffer will be sent to the device.
 */
rtems_status_code rtems_io_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *argument
);

/* Generated from spec:/rtems/io/if/control */

/**
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief Controls the device specified by the device major and minor numbers.
 *
 * @param major is the major number of the device.
 *
 * @param minor is the minor number of the device.
 *
 * @param argument is the argument passed to the device driver I/O control
 *   entry.
 *
 * This directive calls the device driver I/O control entry registered in the
 * Device Driver Table for the specified device major number.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NUMBER The device major number was invalid.
 *
 * @return Other status codes may be returned by the device driver I/O control
 *   entry.
 *
 * @par Notes
 * The exact functionality of the driver entry called by this directive is
 * driver dependent.  It should not be assumed that the control entries of two
 * device drivers are compatible.  For example, an RS-232 driver I/O control
 * operation may change the baud of a serial line, while an I/O control
 * operation for a floppy disk driver may cause a seek operation.
 */
rtems_status_code rtems_io_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                     *argument
);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_IO_H */
