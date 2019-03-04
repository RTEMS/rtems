/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X_clocks
 *
 * @brief System clocks.
 */

/*
 * Copyright (c) 2008, 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC176X_SYSTEM_CLOCKS_H
#define LIBBSP_ARM_LPC176X_SYSTEM_CLOCKS_H

#include <bsp/lpc176x.h>
#include <bsp/timer-defs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc176x_clock System Clocks
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief System clocks.
 *
 * @{
 */

/**
 * @brief Initializes the standard timer.
 *
 * This function uses Timer 1.
 */
void lpc176x_timer_initialize( void );

/**
 * @brief Returns current standard timer value in CPU clocks.
 *
 * @return This function uses Timer 1.
 */
static inline unsigned lpc176x_get_timer1( void )
{
  return LPC176X_T1TC;
}

/**
 * @brief Delay for @a us micro seconds.
 *
 * This function uses the standard timer and assumes that the CPU
 * frequency is in whole MHz numbers.  The delay value @a us will be
 * converted to CPU ticks and there is no protection against integer
 * overflows.
 *
 * This function uses Timer 1.
 */
void lpc176x_micro_seconds_delay( unsigned us );

/**
 * @brief Returns the PLL output clock frequency in [Hz].
 *
 * @return Returns zero in case of an unexpected PLL input frequency.
 */
unsigned lpc176x_pllclk( void );

/**
 * @brief Returns the CPU clock frequency in [Hz].
 *
 * @return Returns zero in case of an unexpected PLL input frequency.
 */
unsigned lpc176x_cclk( void );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC176X_SYSTEM_CLOCKS_H */
