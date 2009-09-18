/**
 * @file
 *
 * @ingroup lpc24xx_clocks
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
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC24XX_SYSTEM_CLOCKS_H
#define LIBBSP_ARM_LPC24XX_SYSTEM_CLOCKS_H

#include <bsp/lpc24xx.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc24xx_clock System Clocks
 *
 * @ingroup lpc24xx
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
void lpc24xx_timer_initialize(void);

/**
 * @brief Returns current standard timer value in CPU clocks.
 *
 * This function uses Timer 1.
 */
static inline unsigned lpc24xx_timer(void)
{
  return T1TC;
}

/**
 * @brief Delay for @a us micro seconds.
 *
 * This function uses the standard timer and assumes that the CPU frequency is
 * in whole MHz numbers.  The delay value @a us will be converted to CPU ticks
 * and there is no protection against integer overflows.
 *
 * This function uses Timer 1.
 */
void lpc24xx_micro_seconds_delay(unsigned us);

/**
 * @brief Returns the PLL output clock frequency in [Hz].
 *
 * Returns zero in case of an unexpected PLL input frequency.
 */
unsigned lpc24xx_pllclk(void);

/**
 * @brief Returns the CPU clock frequency in [Hz].
 *
 * Returns zero in case of an unexpected PLL input frequency.
 */
unsigned lpc24xx_cclk(void);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_SYSTEM_CLOCKS_H */
