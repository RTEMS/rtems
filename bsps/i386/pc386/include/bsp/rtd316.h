/* SPDX-License-Identifier: BSD-2-Clause */

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
uint8_t rtd316_com_get_register(uintptr_t addr, uint8_t reg);

/**
 *  @brief RTD316 Set Register Helper
 *
 *  This method is used to set registers on the RTD316.
 *
 *  @param[in] addr is the base address
 *  @param[in] reg is the register number
 */
void rtd316_com_set_register(uintptr_t addr,uint8_t reg, uint8_t val);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
