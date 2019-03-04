/**
 * @file
 *
 * @ingroup RTEMSBSPsBfinEZKit533
 *
 * @brief Global BSP definitions.
 *
 * This include file contains all board IO definitions for TLL6527M.
 */

/*  bsp.h
 *
 *  This include file contains all board IO definitions for eZKit533.
 *
 *  Copyright (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */


#ifndef LIBBSP_BFIN_EZKIT533_BSP_H
#define LIBBSP_BFIN_EZKIT533_BSP_H

#ifndef ASM

#include <libcpu/bf533.h>

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>
#include <rtems/score/bfin.h>
#include <rtems/bfin/bf533.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSBSPsBfinEZKit533 BF533 EZ-KIT
 *
 * @ingroup RTEMSBSPsBfin
 *
 * @brief BF533 EZ-KIT Board Support Package.
 *
 * @{
 */

/**
 * @name PLL and clock setup values:
 * @brief  PLL configuration for ezkit533
 *
 *  XTL   =  27 MHz
 *  CLKIN =  13 MHz
 *  VCO   = 391 MHz
 *  CCLK  = 391 MHz
 *  SCLK  = 130 MHz
 *
 * @{
 *
 */

#define PLL_CSEL    0x0000      ///< @brief CCLK = VCO      */
#define PLL_SSEL    0x0003      ///< @brief SCLK = CCLK/3   */
#define PLL_MSEL    0x3A00      ///< @brief VCO = 29xCLKIN  */
#define PLL_DF      0x0001      ///< @brief CLKIN = XTL/2   */

#define CCLK        391000000   ///< @brief CORE CLOCK     */
#define SCLK        130000000   ///< @brief SYSTEM CLOCK   */

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
 * @name Ezkit flash ports
 * @{
 */

#define FlashA_PortB_Dir  0x20270007L
#define FlashA_PortB_Data 0x20270005L

/** @} */

/**
 * @brief Blackfin environment memory map
 */
#define L1_DATA_SRAM_A 0xff800000L

#define FIFOLENGTH 0x100

/**
 * @name Constants
 * @{
 */

#define RAM_START 0
#define RAM_END   0x100000

/** @} */

/**
 * @name functions
 * @{
 */

/**
 * @brief Helper Function to use the EzKits LEDS.
 * Can be used by the Application.
 */
void setLED (uint8_t value);

/**
 * @brief Helper Function to use the EzKits LEDS
 */
uint8_t getLED (void);

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

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* !ASM */

/** @} */

#endif
