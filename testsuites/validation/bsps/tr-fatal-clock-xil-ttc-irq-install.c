/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup DevClockXilTtcValFatalIrqInstall
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

#include <bsp.h>
#include <rtems.h>
#include <bsp/fatal.h>
#include <rtems/sysinit.h>

#include "tr-fatal-clock-xil-ttc-irq-install.h"

#include <rtems/test.h>

/**
 * @defgroup DevClockXilTtcValFatalIrqInstall \
 *   spec:/dev/clock/xil-ttc/val/fatal-irq-install
 *
 * @ingroup TestsuitesBspsFatalClockXilTtcIrqInstall
 *
 * @brief Tests a fatal error.
 *
 * This test case performs the following actions:
 *
 * - The test action is carried out by the OccupyClockInterrupt() system
 *   initialization handler.
 *
 *   - Check that the expected fatal source is present.
 *
 *   - Check that the expected fatal code is present.
 *
 * @{
 */

/**
 * @brief Test context for spec:/dev/clock/xil-ttc/val/fatal-irq-install test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   DevClockXilTtcValFatalIrqInstall_Run() parameter.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains a copy of the corresponding
   *   DevClockXilTtcValFatalIrqInstall_Run() parameter.
   */
  rtems_fatal_code code;
} DevClockXilTtcValFatalIrqInstall_Context;

static DevClockXilTtcValFatalIrqInstall_Context
  DevClockXilTtcValFatalIrqInstall_Instance;

static void ClockInterrupt( void *arg )
{
  (void) arg;
}

static rtems_interrupt_entry interrupt_entry = RTEMS_INTERRUPT_ENTRY_INITIALIZER(
  ClockInterrupt,
  NULL,
  "Clock"
);

static void OccupyClockInterrupt( void )
{
  (void) rtems_interrupt_entry_install(
    XIL_CLOCK_TTC_IRQ,
    RTEMS_INTERRUPT_UNIQUE,
    &interrupt_entry
  );
}

RTEMS_SYSINIT_ITEM(
  OccupyClockInterrupt,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_FIRST
);

static T_fixture DevClockXilTtcValFatalIrqInstall_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &DevClockXilTtcValFatalIrqInstall_Instance
};

/**
 * @brief The test action is carried out by the OccupyClockInterrupt() system
 *   initialization handler.
 */
static void DevClockXilTtcValFatalIrqInstall_Action_0(
  DevClockXilTtcValFatalIrqInstall_Context *ctx
)
{
  /* Nothing to do */

  /*
   * Check that the expected fatal source is present.
   */
  T_step_eq_int( 0, ctx->source, RTEMS_FATAL_SOURCE_BSP );

  /*
   * Check that the expected fatal code is present.
   */
  T_step_eq_ulong(
    1,
    ctx->code,
    XIL_FATAL_TTC_IRQ_INSTALL
  );
}

void DevClockXilTtcValFatalIrqInstall_Run(
  rtems_fatal_source source,
  rtems_fatal_code   code
)
{
  DevClockXilTtcValFatalIrqInstall_Context *ctx;

  ctx = &DevClockXilTtcValFatalIrqInstall_Instance;
  ctx->source = source;
  ctx->code = code;

  ctx = T_case_begin(
    "DevClockXilTtcValFatalIrqInstall",
    &DevClockXilTtcValFatalIrqInstall_Fixture
  );

  T_plan( 2 );

  DevClockXilTtcValFatalIrqInstall_Action_0( ctx );

  T_case_end();
}

/** @} */
