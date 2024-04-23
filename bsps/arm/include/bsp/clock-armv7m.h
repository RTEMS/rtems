/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDriverClockArmv7MSysTick
 *
 * @brief This header file provides support for Armv7-M clock drivers.
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
 * Copyright (C) 2011, 2018 Sebastian Huber
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

#ifndef BSP_CLOCK_ARMV7M_H
#define BSP_CLOCK_ARMV7M_H

#include <rtems/score/armv7m.h>
#include <rtems/timecounter.h>

#include <bsp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSDriverClockArmv7MSysTick Armv7-M SysTick Clock Driver
 *
 * @ingroup RTEMSDriverClockImpl
 *
 * @brief This group contains the Armv7-M SysTick support and Clock Driver
 *   implementation.
 *
 * @{
 */

#ifdef ARM_MULTILIB_ARCH_V7M

typedef struct {
  struct timecounter base;
  uint32_t ticks;
} ARMV7M_Timecounter;

extern ARMV7M_Timecounter _ARMV7M_TC;

static inline uint32_t _ARMV7M_Clock_frequency(void)
{
#ifdef BSP_ARMV7M_SYSTICK_FREQUENCY
  return BSP_ARMV7M_SYSTICK_FREQUENCY;
#else
  volatile ARMV7M_Systick *systick = _ARMV7M_Systick;
  return ARMV7M_SYSTICK_CALIB_TENMS_GET(systick->calib) * 100;
#endif
}

static uint32_t _ARMV7M_Clock_counter(ARMV7M_Timecounter *tc)
{
  volatile ARMV7M_Systick *systick;
  rtems_interrupt_level level;
  uint32_t interval;
  uint32_t counter;
  uint32_t ticks;
  uint32_t csr;

  rtems_interrupt_disable(level);
  systick = _ARMV7M_Systick;
  counter = systick->cvr;
  csr = systick->csr;
  interval = systick->rvr;
  ticks = tc->ticks;

  if (RTEMS_PREDICT_FALSE((csr & ARMV7M_SYSTICK_CSR_COUNTFLAG) != 0)) {
    counter = systick->cvr;
    ticks += interval;
    tc->ticks = ticks;
  }

  counter = interval - counter + ticks;
  rtems_interrupt_enable(level);

  return counter;
}

#endif /* ARM_MULTILIB_ARCH_V7M */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP_CLOCK_ARMV7M_H */
