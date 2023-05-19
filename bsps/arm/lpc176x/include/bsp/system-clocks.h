/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X_clocks
 *
 * @brief System clocks.
 */

/*
 * Copyright (C) 2008, 2009 embedded brains GmbH & Co. KG
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
