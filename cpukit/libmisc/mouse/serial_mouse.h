/**
 * @file
 * 
 * @brief Serial Mouse Driver
 *
 * This file describes the Serial Mouse Driver for all boards.
 * This driver assumes that the BSP or application will provide
 * an implementation of the method bsp_get_serial_mouse_device()
 * which tells the driver what serial port device to open() and
 * what type of mouse is connected.
 *
 * This driver relies on the Mouse Parser Engine.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __SERIAL_MOUSE_h__
#define __SERIAL_MOUSE_h__

#include <rtems/io.h>

/* functions */

/**
 *  @defgroup libmisc_serialmouse Serial Mouse Driver
 *  @ingroup libmisc_mouse
 */
/**@{*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Standard device file path for a PS2 mouse device.
 */
#define SERIAL_MOUSE_DEVICE_PS2 "/dev/psaux"

/**
 *  This macro defines the serial mouse device driver entry points.
 */
#define SERIAL_MOUSE_DRIVER_TABLE_ENTRY \
  { serial_mouse_initialize, serial_mouse_open, serial_mouse_close, \
    serial_mouse_read, serial_mouse_write, serial_mouse_control }

/**
 * @brief The initialization of the serial mouse driver.
 *
 * This method initializes the serial mouse driver.
 *
 * @param[in] major is the mouse device major number
 * @param[in] minor is the mouse device minor number
 * @param[in] arg points to device driver arguments
 */
rtems_device_driver serial_mouse_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief Open device driver entry point for the serial mouse driver.
 *
 * This method implements the Open device driver entry
 * point for the serial mouse driver.
 *
 * @param[in] major is the mouse device major number
 * @param[in] minor is the mouse device minor number
 * @param[in] arg points to device driver arguments
 */
rtems_device_driver serial_mouse_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief Close device driver entry point for the serial mouse driver.
 *
 * This method implements the Close device driver entry
 * point for the serial mouse driver.
 *
 * @param[in] major is the mouse device major number
 * @param[in] minor is the mouse device minor number
 * @param[in] arg points to device driver arguments
 */
rtems_device_driver serial_mouse_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief Read device driver entry point for the serial mouse driver.
 *
 * This method implements the Read device driver entry
 * point for the serial mouse driver.
 *
 * @param[in] major is the mouse device major number
 * @param[in] minor is the mouse device minor number
 * @param[in] arg points to device driver arguments
 */
rtems_device_driver serial_mouse_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief Write device driver entry point for the serial mouse driver.
 *
 * This method implements the Write device driver entry
 * point for the serial mouse driver.
 *
 * @param[in] major is the mouse device major number
 * @param[in] minor is the mouse device minor number
 * @param[in] arg points to device driver arguments
 */
rtems_device_driver serial_mouse_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief IO Control device driver entry point for the serial mouse driver.
 *
 * This method implements the IO Control device driver entry
 * point for the serial mouse driver.
 *
 * @param[in] major is the mouse device major number
 * @param[in] minor is the mouse device minor number
 * @param[in] arg points to device driver arguments
 */
rtems_device_driver serial_mouse_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 * @brief Obtain serial mouse configuration information.
 *
 * This method is implemented by the BSP or application and
 * tells the driver what device to open() and what type of
 * mouse is connected.
 *
 * @param[in] name will point to a string with the device name
 *            of the serial port with the mouse connected.
 * @param[in] type will point to a string with the type of mouse connected.
 *
 * @retval This method returns true on success and false on error.
 */
bool bsp_get_serial_mouse_device(
  const char **name,
  const char **type
);

#ifdef __cplusplus
}
#endif
/**@}*/
#endif  /* __tty_drv__  */
