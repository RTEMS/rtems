/**
 *  @file bsp/rtd316.h
 *
 *  @brief RTD316 Driver Interface Definition
 *
 *  This file provides the interface to the RTD316 Dual
 *  serial port utility module.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _BSP_RTD_316_h
#define _BSP_RTD_316_h

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This macro defines the standard device driver table entry for
 *  a console device driver.
 */
#define RTD316_DRIVER_TABLE_ENTRY \
  { rtd316_initialize, NULL, NULL, NULL, NULL, NULL }

/**
 *  @brief RTD316 Initialization Entry Point
 *
 *  This method initializes the RTD316 device driver.
 *
 *  @param[in] major is the device driver major number
 *  @param[in] minor is the device driver minor number
 *  @param[in] arg is the parameters to this call
 *
 *  @return This method returns RTEMS_SUCCESSFUL when
 *          the device driver is successfully initialized.
 */
rtems_device_driver rtd316_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
