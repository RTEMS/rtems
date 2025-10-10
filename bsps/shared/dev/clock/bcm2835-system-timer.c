/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDriverClockImpl
 *
 * @brief This source file contains the implementation of the BCM2835 Clock
 *   Driver.
 */

/*
 * Copyright (C) 2013 Alan Cudmore
 * Copyright (C) 2016 Pavel Pisa
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

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/raspberrypi.h>
#include <rtems/timecounter.h>

static struct timecounter raspberrypi_tc;

static uint32_t raspberrypi_clock_get_timecount(struct timecounter *tc)
{
  (void) tc;

  return BCM2835_REG(BCM2835_GPU_TIMER_CLO);
}

static void raspberrypi_clock_at_tick(void)
{
  uint32_t act_val;
  uint32_t next_cmp = BCM2835_REG(BCM2835_GPU_TIMER_C3);
  next_cmp += rtems_configuration_get_microseconds_per_tick();
  BCM2835_REG(BCM2835_GPU_TIMER_C3) = next_cmp;
  act_val = BCM2835_REG(BCM2835_GPU_TIMER_CLO);

  /*
   * Clear interrupt only if there is time left to the next tick.
   * If time of the next tick has already passed then interrupt
   * request stays active and fires immediately after current tick
   * processing is finished.
   */
  if ((int32_t)(next_cmp - act_val) > 0)
    BCM2835_REG(BCM2835_GPU_TIMER_CS) = BCM2835_GPU_TIMER_CS_M3;
}

static void raspberrypi_clock_handler_install_isr(
  rtems_interrupt_handler clock_isr
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    BCM2835_IRQ_ID_GPU_TIMER_M3,
    "Clock",
    RTEMS_INTERRUPT_UNIQUE,
    clock_isr,
    NULL
  );
  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static void raspberrypi_clock_initialize_hardware(void)
{
  uint32_t next_cmp = BCM2835_REG(BCM2835_GPU_TIMER_CLO);
  next_cmp += rtems_configuration_get_microseconds_per_tick();
  BCM2835_REG(BCM2835_GPU_TIMER_C3) = next_cmp;
  BCM2835_REG(BCM2835_GPU_TIMER_CS) = BCM2835_GPU_TIMER_CS_M3;

  raspberrypi_tc.tc_get_timecount = raspberrypi_clock_get_timecount;
  raspberrypi_tc.tc_counter_mask = 0xffffffff;
  raspberrypi_tc.tc_frequency = 1000000; /* 1 MHz */
  raspberrypi_tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install(&raspberrypi_tc);
}

#define Clock_driver_support_at_tick(arg) raspberrypi_clock_at_tick()

#define Clock_driver_support_initialize_hardware() raspberrypi_clock_initialize_hardware()

#define Clock_driver_support_install_isr(clock_isr) \
  raspberrypi_clock_handler_install_isr(clock_isr)

#define CLOCK_DRIVER_USE_ONLY_BOOT_PROCESSOR 1

#include "../../../shared/dev/clock/clockimpl.h"
