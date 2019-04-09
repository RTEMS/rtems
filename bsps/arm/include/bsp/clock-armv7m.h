/*
 * Copyright (c) 2011, 2018 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef BSP_CLOCK_ARMV7M_H
#define BSP_CLOCK_ARMV7M_H

#include <rtems/score/armv7m.h>
#include <rtems/timecounter.h>

#include <bsp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP_CLOCK_ARMV7M_H */
