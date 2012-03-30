/**
 * @file bsp/exar17d15x.h
 *
 *  This file provides the interface to the Exar Multiport
 *  PCI UART controller.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _BSP_EXAR17D15X__h
#define _BSP_EXAR17D15X__h

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This macro defines the standard device driver table entry for
 *  a console device driver.
 */
#define EXAR17D15X_DRIVER_TABLE_ENTRY \
  { exar17d15x_initialize, NULL, NULL, NULL, NULL, NULL }

/**
 *  @brief Exar 17D15x Initialization Entry Point
 *
 *  This method initializes the Exar XR17D15x device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device driver is successfully initialized.
 */
rtems_device_driver exar17d15x_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
