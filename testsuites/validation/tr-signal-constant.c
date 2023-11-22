/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSignalValSignalConstant
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include "tr-signal-constant.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSignalValSignalConstant \
 *   spec:/rtems/signal/val/signal-constant
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests a signal constant of the @ref RTEMSAPIClassicSignal using the
 *   signal set of the executing task.
 *
 * This test case performs the following actions:
 *
 * - Validate the signal constant.
 *
 *   - Check that the signal constant is equal to the integer representation of
 *     the signal in the signal set.
 *
 * - Validate the signal delivery.
 *
 *   - Check that the caught signal set represents exactly the sent signal.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/signal/val/signal-constant test case.
 */
typedef struct {
  /**
   * @brief This member contains the caught signal set.
   */
  rtems_signal_set signal_set;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsSignalValSignalConstant_Run() parameter.
   */
  rtems_signal_set signal;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsSignalValSignalConstant_Run() parameter.
   */
  int number;
} RtemsSignalValSignalConstant_Context;

static RtemsSignalValSignalConstant_Context
  RtemsSignalValSignalConstant_Instance;

typedef RtemsSignalValSignalConstant_Context Context;

static void SignalHandler( rtems_signal_set signal_set )
{
  Context *ctx;

  ctx = T_fixture_context();
  ctx->signal_set = signal_set;
}

static T_fixture RtemsSignalValSignalConstant_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsSignalValSignalConstant_Instance
};

/**
 * @brief Validate the signal constant.
 */
static void RtemsSignalValSignalConstant_Action_0(
  RtemsSignalValSignalConstant_Context *ctx
)
{
  /* No action */

  /*
   * Check that the signal constant is equal to the integer representation of
   * the signal in the signal set.
   */
  T_step_eq_u32(
    0,
    ctx->signal,
    ( (rtems_signal_set) 1 ) << ctx->number
  );
}

/**
 * @brief Validate the signal delivery.
 */
static void RtemsSignalValSignalConstant_Action_1(
  RtemsSignalValSignalConstant_Context *ctx
)
{
  rtems_status_code sc;

  ctx->signal_set = 0;

  sc = rtems_signal_catch( NULL, RTEMS_DEFAULT_MODES );
  T_step_rsc_success( 1, sc );

  sc = rtems_signal_catch( SignalHandler, RTEMS_NO_ASR );
  T_step_rsc_success( 2, sc );

  sc = rtems_signal_send( RTEMS_SELF, ctx->signal );
  T_step_rsc_success( 3, sc );

  /*
   * Check that the caught signal set represents exactly the sent signal.
   */
  T_step_eq_u32(
    4,
    ctx->signal_set,
    ctx->signal
  );
}

static T_fixture_node RtemsSignalValSignalConstant_Node;

static T_remark RtemsSignalValSignalConstant_Remark = {
  .next = NULL,
  .remark = "RtemsSignalValSignalConstant"
};

void RtemsSignalValSignalConstant_Run( rtems_signal_set signal, int number )
{
  RtemsSignalValSignalConstant_Context *ctx;

  ctx = &RtemsSignalValSignalConstant_Instance;
  ctx->signal = signal;
  ctx->number = number;

  ctx = T_push_fixture(
    &RtemsSignalValSignalConstant_Node,
    &RtemsSignalValSignalConstant_Fixture
  );

  T_plan( 5 );

  RtemsSignalValSignalConstant_Action_0( ctx );
  RtemsSignalValSignalConstant_Action_1( ctx );

  T_add_remark( &RtemsSignalValSignalConstant_Remark );
  T_pop_fixture();
}

/** @} */
