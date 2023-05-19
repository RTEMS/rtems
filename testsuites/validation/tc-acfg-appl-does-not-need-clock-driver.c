/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValApplDoesNotNeedClockDriver
 */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#include <rtems/test.h>

/**
 * @defgroup AcfgValApplDoesNotNeedClockDriver \
 *   spec:/acfg/val/appl-does-not-need-clock-driver
 *
 * @ingroup TestsuitesValidationAcfg0
 *
 * @brief Tests the effect of the
 *   CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER application configuration
 *   option.
 *
 * This test case performs the following actions:
 *
 * - Get the current clock ticks since boot value. Busy wait for at least one
 *   clock tick interval.
 *
 *   - Check that the clock ticks since boot count did not change while busy
 *     waiting for more than one clock tick interval.
 *
 * @{
 */

/**
 * @brief Get the current clock ticks since boot value. Busy wait for at least
 *   one clock tick interval.
 */
static void AcfgValApplDoesNotNeedClockDriver_Action_0( void )
{
  T_time  time_per_clock_tick;
  T_ticks duration;
  T_ticks elapsed;
  T_ticks t0;
  T_ticks t1;
  rtems_interval ticks_since_boot;

  ticks_since_boot = rtems_clock_get_ticks_since_boot();

  time_per_clock_tick = T_seconds_and_nanoseconds_to_time(
    0,
    rtems_configuration_get_nanoseconds_per_tick()
  );
  duration = 2 * T_time_to_ticks( time_per_clock_tick );
  elapsed = 0;
  t0 = T_tick();

  while ( elapsed < duration ) {
    t1 = T_tick();
    elapsed += t1 - t0;
    t0 = t1;
  }

  /*
   * Check that the clock ticks since boot count did not change while busy
   * waiting for more than one clock tick interval.
   */
  T_step_eq_u32(
    0,
    rtems_clock_get_ticks_since_boot(),
    ticks_since_boot
  );
}

/**
 * @fn void T_case_body_AcfgValApplDoesNotNeedClockDriver( void )
 */
T_TEST_CASE( AcfgValApplDoesNotNeedClockDriver )
{
  T_plan( 1 );

  AcfgValApplDoesNotNeedClockDriver_Action_0();
}

/** @} */
