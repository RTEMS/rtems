/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCShared
 *
 * @brief Source file for a clock driver.
 */

/*
 * Copyright (c) 2008-2015 embedded brains GmbH.  All rights reserved.
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
#include <rtems/clockdrv.h>
#include <rtems/timecounter.h>

#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>

#define RTEMS_STATUS_CHECKS_USE_PRINTK

#include <rtems/status-checks.h>

/*
 * This variable must be defined in the BSP and valid before clock driver
 * initialization.
 */
extern uint32_t bsp_time_base_frequency;

#define PPC405_PIT 0x3db

#define PPC_CLOCK_DECREMENTER_MAX UINT32_MAX

volatile uint32_t Clock_driver_ticks = 0;

static uint32_t ppc_clock_decrementer_value = PPC_CLOCK_DECREMENTER_MAX;

static uint32_t ppc_clock_next_time_base;

static struct timecounter ppc_tc;

static uint32_t ppc_get_timecount(struct timecounter *tc)
{
  return ppc_time_base();
}

static void ppc_clock_no_tick(void)
{
  /* Do nothing */
}

static void (*ppc_clock_tick)(void) = ppc_clock_no_tick;

static int ppc_clock_exception_handler(
  BSP_Exception_frame *frame,
  unsigned number
)
{
  uint32_t delta = ppc_clock_decrementer_value;
  uint32_t next = ppc_clock_next_time_base;
  uint32_t dec = 0;
  uint32_t now = 0;
  uint32_t msr = 0;

  do {
    /* Increment clock ticks */
    Clock_driver_ticks += 1;

    /* Enable external exceptions */
    msr = ppc_external_exceptions_enable();

    /* Call clock ticker  */
    ppc_clock_tick();

    /* Restore machine state */
    ppc_external_exceptions_disable( msr);

    /* Next time base */
    next += delta;

    /* Current time */
    now = ppc_time_base();

    /* New decrementer value */
    dec = next - now;
  } while (dec > delta);

  /* Set decrementer */
  ppc_set_decrementer_register( dec);

  /* Expected next time base */
  ppc_clock_next_time_base = next;

  return 0;
}

static int ppc_clock_exception_handler_first(
  BSP_Exception_frame *frame,
  unsigned number
)
{
  /* We have to clear the first pending decrementer exception this way */

  if (ppc_decrementer_register() >= 0x80000000) {
    ppc_clock_exception_handler( frame, number);
  }

  ppc_exc_set_handler( ASM_DEC_VECTOR, ppc_clock_exception_handler);

  return 0;
}

static int ppc_clock_exception_handler_booke(
  BSP_Exception_frame *frame,
  unsigned number
)
{
  uint32_t msr;

  /* Acknowledge decrementer request */
  PPC_SET_SPECIAL_PURPOSE_REGISTER( BOOKE_TSR, BOOKE_TSR_DIS);

  /* Increment clock ticks */
  Clock_driver_ticks += 1;

  /* Enable external exceptions */
  msr = ppc_external_exceptions_enable();

  /* Call clock ticker  */
  ppc_clock_tick();

  /* Restore machine state */
  ppc_external_exceptions_disable( msr);

  return 0;
}

static int ppc_clock_exception_handler_ppc405(BSP_Exception_frame *frame, unsigned number)
{
  uint32_t msr;

  /* Acknowledge PIT request */
  PPC_SET_SPECIAL_PURPOSE_REGISTER(PPC405_TSR, BOOKE_TSR_DIS);

  /* Increment clock ticks */
  Clock_driver_ticks += 1;

  /* Enable external exceptions */
  msr = ppc_external_exceptions_enable();

  /* Call clock ticker  */
  ppc_clock_tick();

  /* Restore machine state */
  ppc_external_exceptions_disable(msr);

  return 0;
}

void _Clock_Initialize( void )
{
  uint64_t frequency = bsp_time_base_frequency;
  uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
  uint32_t interval = (uint32_t) ((frequency * us_per_tick) / 1000000);

  /*
   * Set default ticker.
   */
  ppc_clock_tick = rtems_timecounter_tick;

  if (ppc_cpu_is_bookE() != PPC_BOOKE_405) {
    /* Decrementer value */
    ppc_clock_decrementer_value = interval - 1;

    /* Check decrementer value */
    if (ppc_clock_decrementer_value == 0) {
      ppc_clock_decrementer_value = PPC_CLOCK_DECREMENTER_MAX;
      RTEMS_SYSLOG_ERROR( "decrementer value would be zero, will be set to maximum value instead\n");
    }
    if (ppc_cpu_is_bookE()) {
      /* Set decrementer auto-reload value */
      PPC_SET_SPECIAL_PURPOSE_REGISTER( BOOKE_DECAR, ppc_clock_decrementer_value);

      /* Install exception handler */
      ppc_exc_set_handler( ASM_BOOKE_DEC_VECTOR, ppc_clock_exception_handler_booke);

      /* Enable decrementer and auto-reload */
      PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS( BOOKE_TCR, BOOKE_TCR_DIE | BOOKE_TCR_ARE);
    } else {
      /* Here the decrementer value is actually the interval */
      ++ppc_clock_decrementer_value;

      /* Initialize next time base */
      ppc_clock_next_time_base = ppc_time_base() + ppc_clock_decrementer_value;

      /* Install exception handler */
      ppc_exc_set_handler( ASM_DEC_VECTOR, ppc_clock_exception_handler_first);
    }

    /* Set the decrementer value */
    ppc_set_decrementer_register( ppc_clock_decrementer_value);
  } else {
    /* PIT interval value */
    ppc_clock_decrementer_value = interval;

    /* Install exception handler */
    ppc_exc_set_handler(ASM_BOOKE_DEC_VECTOR, ppc_clock_exception_handler_ppc405);

    /* Enable PIT and auto-reload */
    PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS(PPC405_TCR, BOOKE_TCR_DIE | BOOKE_TCR_ARE);

    /* Set PIT auto-reload and initial value */
    PPC_SET_SPECIAL_PURPOSE_REGISTER(PPC405_PIT, interval);
  }

  /* Install timecounter */
  ppc_tc.tc_get_timecount = ppc_get_timecount;
  ppc_tc.tc_counter_mask = 0xffffffff;
  ppc_tc.tc_frequency = frequency;
  ppc_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&ppc_tc);
}
