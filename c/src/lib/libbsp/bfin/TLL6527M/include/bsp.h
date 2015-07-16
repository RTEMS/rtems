/**
 * @file bsp.h
 * @ingroup bfin_tll6527m
 * @brief Global BSP definitions.
 *
 * This include file contains all board IO definitions for TLL6527M.
 */

/*
 * COPYRIGHT (c) 2010 by ECE Northeastern University.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license
 */

#ifndef LIBBSP_BFIN_TLL6527M_BSP_H
#define LIBBSP_BFIN_TLL6527M_BSP_H

#ifndef ASM

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <rtems/score/bfin.h>
#include <rtems/bfin/bf52x.h>
#include <bf52x.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup bfin_tll6527m TLL6527M Support
 * @ingroup bsp_bfin
 * @brief TLL6527M Support Package
 * @{
 */

/*
 * PLL and clock setup values:
 */

/*
 *  PLL configuration for TLL6527M
 *
 *  XTL   =  27 MHz
 *  CLKIN =  13 MHz
 *  VCO   = 391 MHz
 *  CCLK  = 391 MHz
 *  SCLK  = 130 MHz
 */

/**
 * @name PLL Configuration
 * @{
 */

#define PLL_CSEL    0x0000      ///< @brief CCLK = VCO      */
#define PLL_SSEL    0x0003      ///< @brief SCLK = CCLK/3   */
#define PLL_MSEL    0x3A00      ///< @brief VCO = 29xCLKIN  */
#define PLL_DF      0x0001      ///< @brief CLKIN = XTL/2   */

/** @} */

/**
 * @name Clock setup values
 * @{
 */

#define CLKIN		    (25000000)  ///< @brief Input clock to the PLL */
#define CCLK        (600000000)   ///< @brief CORE CLOCK     */
#define SCLK        (100000000)   ///< @brief SYSTEM CLOCK   */

/** @} */

/**
 * @name UART setup values
 * @{
 */

#define BAUDRATE    57600       ///< @brief Console Baudrate   */
#define WORD_5BITS  0x00        ///< @brief 5 bits word        */
#define WORD_6BITS  0x01        ///< @brief 6 bits word        */
#define WORD_7BITS  0x02        ///< @brief 7 bits word        */
#define WORD_8BITS  0x03        ///< @brief 8 bits word        */
#define EVEN_PARITY 0x18        ///< @brief Enable EVEN parity */
#define ODD_PARITY  0x08        ///< @brief Enable ODD parity  */
#define TWO_STP_BIT 0x04        ///< @brief 2 stop bits        */

/** @} */

/**
 * @brief Install an interrupt handler
 *
 * This method installs an interrupt handle.
 *
 * @param[in] handler is the isr routine
 * @param[in] vector is the vector number
 * @param[in] type indicates whether RTEMS or RAW intr
 *
 * @return returns old vector
 */
rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

/*
 *  Internal BSP methods that are used across file boundaries
 */
void Init_RTC(void);

/*
 * Prototype for methods in .S files that are referenced from C.
 */
void bfin_null_isr(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

#endif
