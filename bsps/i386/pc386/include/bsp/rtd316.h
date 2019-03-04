/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
 *
 * @brief RTD316 driver interface defintions.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

/**
 *  @brief RTD316 Obtain Register Helper
 *
 *  This method is used to read registers on the RTD316.
 *
 *  @param[in] addr is the base address
 *  @param[in] reg is the register number
 *
 *  @return This method returns the value of the register.
 */
uint8_t rtd316_com_get_register(uint32_t addr, uint8_t reg);

/**
 *  @brief RTD316 Set Register Helper
 *
 *  This method is used to set registers on the RTD316.
 *
 *  @param[in] addr is the base address
 *  @param[in] reg is the register number
 */
void rtd316_com_set_register(uint32_t addr,uint8_t reg, uint8_t val);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
