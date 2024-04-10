/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup DevClockXilTtcValTickCatchUp
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <sys/time.h>

#include <rtems/test.h>

/**
 * @defgroup DevClockXilTtcValTickCatchUp \
 *   spec:/dev/clock/xil-ttc/val/tick-catch-up
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @brief Tests some Xilinx TTC clock driver functions.
 *
 * This test case performs the following actions:
 *
 * - Synchronize with the clock tick.  Disable interrupts.  Busy wait three
 *   clock tick intervals.  Enable interrupts.
 *
 *   - Check that exactly three clock ticks happened once interrupts are
 *     enabled again.
 *
 * @{
 */

/**
 * @brief Synchronize with the clock tick.  Disable interrupts.  Busy wait
 *   three clock tick intervals.  Enable interrupts.
 */
static void DevClockXilTtcValTickCatchUp_Action_0( void )
{
  uint32_t              ns_per_tick;
  uint64_t              three_ticks_interval;
  rtems_interrupt_level level;
  rtems_interval        t_0;
  rtems_interval        t_1;
  rtems_interval        t_2;
  uint64_t              m_0;
  uint64_t              m_1;

  ns_per_tick = rtems_configuration_get_nanoseconds_per_tick();
  three_ticks_interval = ( 7 * (uint64_t) nstosbt( ns_per_tick ) ) / 2;
  t_0 = rtems_clock_get_ticks_since_boot();

  /* Synchronize with clock tick */
  do {
    t_1 = rtems_clock_get_ticks_since_boot();
    m_0 = (uint64_t) rtems_clock_get_monotonic_sbintime();
  } while ( t_0 == t_1 );

  rtems_interrupt_local_disable( level );

  do {
    m_1 = (uint64_t) rtems_clock_get_monotonic_sbintime();
  } while ( m_1 - m_0 <= three_ticks_interval );

  rtems_interrupt_local_enable( level );

  /*
   * Make sure the clock interrupt was serviced after the interrupt enable.
   */
  do {
    t_2 = rtems_clock_get_ticks_since_boot();
  } while ( t_1 == t_2 );

  /*
   * Check that exactly three clock ticks happened once interrupts are enabled
   * again.
   */
  T_step_eq_u32( 0, t_2 - t_1, 3 );
}

/**
 * @fn void T_case_body_DevClockXilTtcValTickCatchUp( void )
 */
T_TEST_CASE( DevClockXilTtcValTickCatchUp )
{
  T_plan( 1 );

  DevClockXilTtcValTickCatchUp_Action_0();
}

/** @} */
