/**
 * @file rtems/serial_mouse.h
 *
 *  This file describes the Serial Mouse Driver for all boards.
 *  This driver assumes that the BSP or application will provide
 *  an implementation of the method bsp_get_serial_mouse_device()
 *  which tells the driver what serial port device to open() and
 *  what type of mouse is connected.
 *
 *  This driver relies on the Mouse Parser Engine.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __SERIAL_MOUSE_h__
#define __SERIAL_MOUSE_h__

/* functions */
#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This macro defines the serial mouse device driver entry points.
 */
#define SERIAL_MOUSE_DRIVER_TABLE_ENTRY \
  { serial_mouse_initialize, serial_mouse_open, serial_mouse_close, \
    serial_mouse_read, serial_mouse_write, serial_mouse_control }

/**
 *  @brief Serial Mouse Driver Initialization
 *
 *  This method initializes the serial mouse driver.
 *
 *  @param[in] major is the mouse device major number
 *  @param[in] minor is the mouse device minor number
 *  @param[in] arguments points to device driver arguments
 */
rtems_device_driver serial_mouse_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Serial Mouse Driver Open
 *
 *  This method implements the Open device driver entry
 *  point for the serial mouse driver.
 *
 *  @param[in] major is the mouse device major number
 *  @param[in] minor is the mouse device minor number
 *  @param[in] arguments points to device driver arguments
 */
rtems_device_driver serial_mouse_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Serial Mouse Driver Close
 *
 *  This method implements the Close device driver entry
 *  point for the serial mouse driver.
 *
 *  @param[in] major is the mouse device major number
 *  @param[in] minor is the mouse device minor number
 *  @param[in] arguments points to device driver arguments
 */
rtems_device_driver serial_mouse_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Serial Mouse Driver Read
 *
 *  This method implements the Read device driver entry
 *  point for the serial mouse driver.
 *
 *  @param[in] major is the mouse device major number
 *  @param[in] minor is the mouse device minor number
 *  @param[in] arguments points to device driver arguments
 */
rtems_device_driver serial_mouse_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Serial Mouse Driver Write
 *
 *  This method implements the Write device driver entry
 *  point for the serial mouse driver.
 *
 *  @param[in] major is the mouse device major number
 *  @param[in] minor is the mouse device minor number
 *  @param[in] arguments points to device driver arguments
 */
rtems_device_driver serial_mouse_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Serial Mouse Driver IO Control
 *
 *  This method implements the IO Control device driver entry
 *  point for the serial mouse driver.
 *
 *  @param[in] major is the mouse device major number
 *  @param[in] minor is the mouse device minor number
 *  @param[in] arguments points to device driver arguments
 */
rtems_device_driver serial_mouse_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Obtain Serial Mouse Configuration Information
 *
 *  This method is implemented by the BSP or application and
 *  tells the driver what device to open() and what type of
 *  mouse is connected.
 *
 *  @param[in] name will point to a string with the device name
 *             of the serial port with the mouse connected.
 *  @param[in] type will point to a string with the type of mouse connected.
 *
 *  @return This method returns true on success and false on error.
 */
bool bsp_get_serial_mouse_device(
  const char **name,
  const char **type
);

#ifdef __cplusplus
}
#endif

#endif  /* __tty_drv__  */
