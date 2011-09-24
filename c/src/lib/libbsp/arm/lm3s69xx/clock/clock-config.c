/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <rtems.h>
#include <rtems/score/armv7m.h>

/* This is defined in ../../../shared/clockdrv_shell.h */
rtems_isr Clock_isr(rtems_vector_number vector);

static uint64_t _ARMV7M_Systick_factor;

static void _ARMV7M_Systick_at_tick(void)
{
  volatile ARMV7M_Systick *systick = _ARMV7M_Systick;

  /* Clear COUNTFLAG */
  systick->csr;
}

static void _ARMV7M_Systick_handler(void)
{
  _ARMV7M_Interrupt_service_enter();
  Clock_isr(ARMV7M_VECTOR_SYSTICK);
  _ARMV7M_Interrupt_service_leave();
}

static void _ARMV7M_Systick_handler_install(void)
{
  _ARMV7M_Set_exception_handler(
    ARMV7M_VECTOR_SYSTICK,
    _ARMV7M_Systick_handler
  );
}

static void _ARMV7M_Systick_initialize(void)
{
  volatile ARMV7M_Systick *systick = _ARMV7M_Systick;
  uint64_t frequency = ARMV7M_SYSTICK_CALIB_TENMS(systick->calib) * 100ULL;
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint64_t interval = (frequency * us_per_tick) / 1000000ULL;

  _ARMV7M_Systick_factor = (1000000000ULL << 32) / frequency;

  systick->rvr = (uint32_t) interval;
  systick->cvr = 0;
  systick->csr = ARMV7M_SYSTICK_CSR_TICKINT | ARMV7M_SYSTICK_CSR_ENABLE;
}

static void _ARMV7M_Systick_cleanup(void)
{
  volatile ARMV7M_Systick *systick = _ARMV7M_Systick;

  systick->csr = 0;
}

static uint32_t _ARMV7M_Systick_nanoseconds_since_last_tick(void)
{
  volatile ARMV7M_Systick *systick = _ARMV7M_Systick;
  volatile ARMV7M_SCB *scb = _ARMV7M_SCB;
  uint32_t rvr = systick->rvr;
  uint32_t c = rvr - systick->cvr;

  if ((scb->icsr & ARMV7M_SCB_ICSR_PENDSTSET) != 0) {
    c = rvr - systick->cvr + rvr;
  }

  return (uint32_t) ((c * _ARMV7M_Systick_factor) >> 32);
}

#define Clock_driver_support_at_tick() \
  _ARMV7M_Systick_at_tick()

#define Clock_driver_support_initialize_hardware() \
  _ARMV7M_Systick_initialize()

#define Clock_driver_support_install_isr(isr, old_isr) \
  do { \
    _ARMV7M_Systick_handler_install(); \
    old_isr = NULL; \
  } while (0)

#define Clock_driver_support_shutdown_hardware() \
  _ARMV7M_Systick_cleanup()

#define Clock_driver_nanoseconds_since_last_tick \
  _ARMV7M_Systick_nanoseconds_since_last_tick

/* Include shared source clock driver code */
#include "../../../shared/clockdrv_shell.h"
