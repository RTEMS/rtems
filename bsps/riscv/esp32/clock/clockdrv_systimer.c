/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDriverClockImpl
 *
 * @brief This source file contains the implementation of the ESP32 SYSTIMER
 *   Clock Driver.
 */

/*
 * Copyright (C) 2026 Kinsey Moore <wkmoore@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp/fatal.h>
#include <bsp/irq.h>
#include <bsp/utility.h>

#include <rtems/sysinit.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/percpu.h>
#include <rtems/score/riscv-utility.h>
#include <rtems/score/smpimpl.h>
#include <rtems/timecounter.h>

typedef struct {
  struct timecounter base;
  uint32_t           interval;
} systimer_timecounter;

static systimer_timecounter systimer_clock_tc;

static void systimer_clock_handler_install( rtems_interrupt_handler handler )
{
  rtems_status_code sc;

  sc = rtems_interrupt_handler_install(
    SYSTIMER_TARGET0_INTR,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    handler,
    &systimer_clock_tc
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    bsp_fatal( RISCV_FATAL_CLOCK_IRQ_INSTALL );
  }
}

#define SYSTIMER_CONF_REG                   0x0
#define SYSTIMER_CLK_EN                     BSP_BIT32( 31 )
#define SYSTIMER_TIMER_UNIT0_WORK_EN        BSP_BIT32( 30 )
#define SYSTIMER_TIMER_UNIT0_CORE0_STALL_EN BSP_BIT32( 28 )
#define SYSTIMER_TARGET0_WORK_EN            BSP_BIT32( 24 )

#define SYSTIMER_UNIT0_OP_REG            0x4
#define SYSTIMER_TIMER_UNIT0_UPDATE      BSP_BIT32( 30 )
#define SYSTIMER_TIMER_UNIT0_VALUE_VALID BSP_BIT32( 29 )

#define SYSTIMER_UNIT0_LOAD_HI_REG 0xc
#define SYSTIMER_UNIT0_LOAD_LO_REG 0x10

#define SYSTIMER_TARGET0_HI_REG 0x1c
#define SYSTIMER_TARGET0_LO_REG 0x20

#define SYSTIMER_TARGET0_CONF_REG               0x34
#define SYSTIMER_TARGET0_TIMER_UNIT_SEL         BSP_BIT32( 31 )
#define SYSTIMER_TARGET0_PERIOD_MODE            BSP_BIT32( 30 )
#define SYSTIMER_TARGET0_PERIOD( val )          BSP_FLD32( val, 0, 25 )
#define SYSTIMER_TARGET0_PERIOD_GET( reg )      BSP_FLD32GET( reg, 0, 25 )
#define SYSTIMER_TARGET0_PERIOD_SET( reg, val ) BSP_FLD32SET( reg, val, 0, 25 )

#define SYSTIMER_UNIT0_VALUE_HI_REG 0x40
#define SYSTIMER_UNIT0_VALUE_LO_REG 0x44

#define SYSTIMER_COMP0_LOAD_REG 0x50
#define SYSTIMER_COMP0_LOAD     BSP_BIT32( 0 )

#define SYSTIMER_UNIT0_LOAD_REG   0x5c
#define SYSTIMER_TIMER_UNIT0_LOAD BSP_BIT32( 0 )

#define SYSTIMER_INT_ENA_REG     0x64
#define SYSTIMER_TARGET0_INT_ENA BSP_BIT32( 0 )

#define SYSTIMER_INT_RAW_REG     0x68
#define SYSTIMER_TARGET0_INT_RAW BSP_BIT32( 0 )

#define SYSTIMER_INT_CLR_REG     0x6c
#define SYSTIMER_TARGET0_INT_CLR BSP_BIT32( 0 )

#define SYSTIMER_INT_ST_REG     0x70
#define SYSTIMER_TARGET0_INT_ST BSP_BIT32( 0 )

#define SYSTIMER_REG( reg ) *( (volatile uint32_t *) ( SYSTIMER_BASE + reg ) )

/* The SYSTIMER always operates at 16MHz */
#define SYSTIMER_FREQUENCY ( 16ULL * 1024 * 1024 )

static uint32_t systimer_get_timecount( struct timecounter *arg )
{
  (void) arg;
  return _CPU_Counter_read();
}

static void systimer_clock_initialize( void )
{
  /* unit0 is selected by default */
  uint32_t target0_conf = 0;

  /* us/interval * count/second * 1s/1000000us == count/interval */
  uint32_t us_per_tick = rtems_configuration_get_microseconds_per_tick();

  /* 1us/interval * 16Mcount/second * 1s/1000000us == 16count/interval */
  systimer_clock_tc.interval = (uint32_t) ( ( us_per_tick *
                                              SYSTIMER_FREQUENCY ) /
                                            1000000 );

  /* Set target0 period, unit0 is selected by default */
  target0_conf = SYSTIMER_TARGET0_PERIOD( systimer_clock_tc.interval );

  /* Select target0 period mode */
  target0_conf |= SYSTIMER_TARGET0_PERIOD_MODE;

  /* SYSTEM_SYSTIMER_CLK_EN in SYSTEM_PERIP_CLK_EN0_REG is 1 on reset */

  /* Clear Target0 Configuration register */
  SYSTIMER_REG( SYSTIMER_TARGET0_CONF_REG ) = 0;

  /* Set Target0 Configuration register */
  SYSTIMER_REG( SYSTIMER_TARGET0_CONF_REG ) = target0_conf;

  /* load target0 period and unit selection into hardware */
  SYSTIMER_REG( SYSTIMER_COMP0_LOAD_REG ) = SYSTIMER_COMP0_LOAD;

  /* enable COMP0 */
  SYSTIMER_REG( SYSTIMER_CONF_REG ) |= SYSTIMER_TARGET0_WORK_EN;

  /* enable TARGET0 interrupt */
  SYSTIMER_REG( SYSTIMER_INT_ENA_REG ) = SYSTIMER_TARGET0_INT_ENA;

  /* install timercounter */
  systimer_clock_tc.base.tc_get_timecount = systimer_get_timecount;
  systimer_clock_tc.base.tc_counter_mask = 0xffffffff;
  systimer_clock_tc.base.tc_frequency = SYSTIMER_FREQUENCY;
  systimer_clock_tc.base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install( &systimer_clock_tc.base );
}

uint32_t _CPU_Counter_frequency( void )
{
  return SYSTIMER_FREQUENCY;
}

static uint32_t systimer_unit0_value_valid( void )
{
  uint32_t unit0_op = SYSTIMER_REG( SYSTIMER_UNIT0_OP_REG );
  return unit0_op & SYSTIMER_TIMER_UNIT0_VALUE_VALID;
}

CPU_Counter_ticks _CPU_Counter_read( void )
{
  /* initiate value transfer to readable register */
  SYSTIMER_REG( SYSTIMER_UNIT0_OP_REG ) = SYSTIMER_TIMER_UNIT0_UPDATE;

  /* wait for updated value, typically takes 0-2 loop iterations */
  while ( systimer_unit0_value_valid() == 0 ) {
    ;
  }

  /*
   * only return the low 32 bits because CPU_Counter_ticks is only 32bit and
   * the configured period must be less than this
   */
  return SYSTIMER_REG( SYSTIMER_UNIT0_VALUE_LO_REG );
}

static void systimer_clock_at_tick( void )
{
  /* acknowledge the interrupt */
  SYSTIMER_REG( SYSTIMER_INT_CLR_REG ) = SYSTIMER_TARGET0_INT_CLR;
}

#define Clock_driver_support_initialize_hardware() systimer_clock_initialize()

#define Clock_driver_support_install_isr( isr ) \
  systimer_clock_handler_install( isr )

#define Clock_driver_support_at_tick( arg ) systimer_clock_at_tick()

#include "../../../shared/dev/clock/clockimpl.h"
