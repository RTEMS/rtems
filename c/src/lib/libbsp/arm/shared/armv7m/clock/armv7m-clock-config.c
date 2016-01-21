/*
 * Copyright (c) 2011-2012 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/timecounter.h>
#include <rtems/score/armv7m.h>

#include <bsp.h>

#ifdef ARM_MULTILIB_ARCH_V7M

/* This is defined in clockdrv_shell.h */
static void Clock_isr(void *arg);

typedef struct {
  rtems_timecounter_simple base;
  void (*tick)(void);
  bool countflag;
} ARMV7M_Timecounter;

static ARMV7M_Timecounter _ARMV7M_TC;

static uint32_t _ARMV7M_TC_systick_get(rtems_timecounter_simple *tc)
{
  volatile ARMV7M_Systick *systick = _ARMV7M_Systick;

  return systick->cvr;
}

static bool _ARMV7M_TC_systick_is_pending(rtems_timecounter_simple *base)
{
  ARMV7M_Timecounter *tc = (ARMV7M_Timecounter *) base;
  rtems_interrupt_level level;
  bool countflag;

  rtems_interrupt_disable(level);

  countflag = tc->countflag;
  if (!countflag) {
    volatile ARMV7M_Systick *systick = _ARMV7M_Systick;

    countflag = ((systick->csr & ARMV7M_SYSTICK_CSR_COUNTFLAG) != 0);
    tc->countflag = countflag;
  }

  rtems_interrupt_enable(level);

  return countflag;
}

static uint32_t _ARMV7M_TC_systick_get_timecount(struct timecounter *tc)
{
  return rtems_timecounter_simple_downcounter_get(
    tc,
    _ARMV7M_TC_systick_get,
    _ARMV7M_TC_systick_is_pending
  );
}

static void _ARMV7M_TC_systick_at_tick(rtems_timecounter_simple *base)
{
  ARMV7M_Timecounter *tc = (ARMV7M_Timecounter *) base;
  volatile ARMV7M_Systick *systick = _ARMV7M_Systick;

  tc->countflag = false;

  /* Clear COUNTFLAG */
  systick->csr;
}

static void _ARMV7M_TC_systick_tick(void)
{
  rtems_timecounter_simple_downcounter_tick(
    &_ARMV7M_TC.base,
    _ARMV7M_TC_systick_get,
    _ARMV7M_TC_systick_at_tick
  );
}

static uint32_t _ARMV7M_TC_dwt_get_timecount(struct timecounter *tc)
{
  volatile ARMV7M_DWT *dwt = _ARMV7M_DWT;

  return dwt->cyccnt;
}

static void _ARMV7M_TC_dwt_tick(void)
{
  rtems_timecounter_tick();
}

static void _ARMV7M_TC_tick(void)
{
  (*_ARMV7M_TC.tick)();
}

static void _ARMV7M_Systick_handler(void)
{
  _ARMV7M_Interrupt_service_enter();
  Clock_isr(NULL);
  _ARMV7M_Interrupt_service_leave();
}

static void _ARMV7M_Systick_handler_install(void)
{
  _ARMV7M_Set_exception_priority_and_handler(
    ARMV7M_VECTOR_SYSTICK,
    BSP_ARMV7M_SYSTICK_PRIORITY,
    _ARMV7M_Systick_handler
  );
}

static void _ARMV7M_Systick_initialize(void)
{
  volatile ARMV7M_DWT *dwt = _ARMV7M_DWT;
  volatile ARMV7M_Systick *systick = _ARMV7M_Systick;
  #ifdef BSP_ARMV7M_SYSTICK_FREQUENCY
    uint64_t freq = BSP_ARMV7M_SYSTICK_FREQUENCY;
  #else
    uint64_t freq = ARMV7M_SYSTICK_CALIB_TENMS_GET(systick->calib) * 100ULL;
  #endif
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint64_t interval = (freq * us_per_tick) / 1000000ULL;
  uint32_t dwt_ctrl;

  systick->rvr = (uint32_t) interval;
  systick->cvr = 0;
  systick->csr = ARMV7M_SYSTICK_CSR_ENABLE
    | ARMV7M_SYSTICK_CSR_TICKINT
    | ARMV7M_SYSTICK_CSR_CLKSOURCE;

  dwt_ctrl = dwt->ctrl;
  if ((dwt_ctrl & ARMV7M_DWT_CTRL_NOCYCCNT) == 0) {
    dwt->ctrl = dwt_ctrl | ARMV7M_DWT_CTRL_CYCCNTENA;
    _ARMV7M_TC.base.tc.tc_get_timecount = _ARMV7M_TC_dwt_get_timecount;
    _ARMV7M_TC.base.tc.tc_counter_mask = 0xffffffff;
    _ARMV7M_TC.base.tc.tc_frequency = freq;
    _ARMV7M_TC.base.tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
    _ARMV7M_TC.tick = _ARMV7M_TC_dwt_tick;
    rtems_timecounter_install(&_ARMV7M_TC.base.tc);
  } else {
    _ARMV7M_TC.tick = _ARMV7M_TC_systick_tick;
    rtems_timecounter_simple_install(
      &_ARMV7M_TC.base,
      freq,
      interval,
      _ARMV7M_TC_systick_get_timecount
    );
  }
}

static void _ARMV7M_Systick_cleanup(void)
{
  volatile ARMV7M_Systick *systick = _ARMV7M_Systick;

  systick->csr = 0;
}

#define Clock_driver_timecounter_tick() _ARMV7M_TC_tick()

#define Clock_driver_support_initialize_hardware() \
  _ARMV7M_Systick_initialize()

#define Clock_driver_support_install_isr(isr, old_isr) \
  do { \
    _ARMV7M_Systick_handler_install(); \
    old_isr = NULL; \
  } while (0)

#define Clock_driver_support_shutdown_hardware() \
  _ARMV7M_Systick_cleanup()

/* Include shared source clock driver code */
#include "../../../../shared/clockdrv_shell.h"

#endif /* ARM_MULTILIB_ARCH_V7M */
