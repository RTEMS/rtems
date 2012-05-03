/**
 * @file termios_testdriver.h
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _TERMIOS_TESTDRIVER_H
#define _TERMIOS_TESTDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

void termios_test_driver_set_rx(
  const void *p,
  size_t      len
);

void termios_test_driver_set_rx_enqueue_now(
  bool value
);

void termios_test_driver_dump_tx(const char *c);

/**
 *  This macro defines the standard name for the Termios Test device
 *  that is available to applications.
 */
#define TERMIOS_TEST_DRIVER_DEVICE_NAME "/dev/test"

/**
 *  This macro defines the standard device driver table entry for
 *  a Termios Test device driver.
 */
#define TERMIOS_TEST_DRIVER_TABLE_ENTRY \
  { termios_test_driver_initialize, termios_test_driver_open, \
    termios_test_driver_close, termios_test_driver_read, \
    termios_test_driver_write, termios_test_driver_control }

/**
 *  @brief Console Initialization Entry Point
 *
 *  This method initializes the Termios Test device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device driver is successfully initialized.
 */
rtems_device_driver termios_test_driver_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Console Open Entry Point
 *
 *  This method opens a specific device supported by the
 *  Termios Test device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device driver is successfully opened.
 */
rtems_device_driver termios_test_driver_open(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Console Close Entry Point
 *
 *  This method closes a specific device supported by the
 *  Termios Test device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device is successfully closed.
 */
rtems_device_driver termios_test_driver_close(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Console Read Entry Point
 *
 *  This method reads from a specific device supported by the
 *  Termios Test device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device is successfully read from.
 */
rtems_device_driver termios_test_driver_read(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Console Write Entry Point
 *
 *  This method writes to a specific device supported by the
 *  Termios Test device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device is successfully written.
 */
rtems_device_driver termios_test_driver_write(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

/**
 *  @brief Console IO Control Entry Point
 *
 *  This method performs an IO Control operation on a
 *  specific device supported by the Termios Test device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device driver IO control operation is
 *          successfully performed.
 */
rtems_device_driver termios_test_driver_control(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
