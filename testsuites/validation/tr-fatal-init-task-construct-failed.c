/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValFatalInitTaskConstructFailed
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
#include <rtems/rtems/tasksdata.h>

#include "tr-fatal-init-task-construct-failed.h"

#include <rtems/test.h>

/**
 * @defgroup AcfgValFatalInitTaskConstructFailed \
 *   spec:/acfg/val/fatal-init-task-construct-failed
 *
 * @ingroup TestsuitesFatalInitTaskConstructFailed
 *
 * @brief Tests a fatal error caused by an invalid application configuration.
 *
 * This test case performs the following actions:
 *
 * - The test action is carried out by configuring an invalid task priority
 *   used to construct the initialization task.
 *
 *   - Check that the expected fatal source is present.
 *
 *   - Check that the expected fatal code is present.
 *
 *   - Check that the CONFIGURE_INIT_TASK_PRIORITY application configuration
 *     option resulted in the expected system setting.
 *
 * @{
 */

/**
 * @brief Test context for spec:/acfg/val/fatal-init-task-construct-failed test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   AcfgValFatalInitTaskConstructFailed_Run() parameter.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains a copy of the corresponding
   *   AcfgValFatalInitTaskConstructFailed_Run() parameter.
   */
  rtems_fatal_code code;
} AcfgValFatalInitTaskConstructFailed_Context;

static AcfgValFatalInitTaskConstructFailed_Context
  AcfgValFatalInitTaskConstructFailed_Instance;

static T_fixture AcfgValFatalInitTaskConstructFailed_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &AcfgValFatalInitTaskConstructFailed_Instance
};

/**
 * @brief The test action is carried out by configuring an invalid task
 *   priority used to construct the initialization task.
 */
static void AcfgValFatalInitTaskConstructFailed_Action_0(
  AcfgValFatalInitTaskConstructFailed_Context *ctx
)
{
  /* Nothing to do */

  /*
   * Check that the expected fatal source is present.
   */
  T_step_eq_int( 0, ctx->source, INTERNAL_ERROR_CORE );

  /*
   * Check that the expected fatal code is present.
   */
  T_step_eq_ulong(
    1,
    ctx->code,
    INTERNAL_ERROR_RTEMS_INIT_TASK_CONSTRUCT_FAILED
  );

  /*
   * Check that the CONFIGURE_INIT_TASK_PRIORITY application configuration
   * option resulted in the expected system setting.
   */
  T_step_eq_u32(
    2,
    _RTEMS_tasks_User_task_config.config.initial_priority,
    0
  );
}

void AcfgValFatalInitTaskConstructFailed_Run(
  rtems_fatal_source source,
  rtems_fatal_code   code
)
{
  AcfgValFatalInitTaskConstructFailed_Context *ctx;

  ctx = &AcfgValFatalInitTaskConstructFailed_Instance;
  ctx->source = source;
  ctx->code = code;

  ctx = T_case_begin(
    "AcfgValFatalInitTaskConstructFailed",
    &AcfgValFatalInitTaskConstructFailed_Fixture
  );

  T_plan( 3 );

  AcfgValFatalInitTaskConstructFailed_Action_0( ctx );

  T_case_end();
}

/** @} */
