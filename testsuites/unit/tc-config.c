/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsConfigUnitConfig
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
#include <rtems/rtems/semdata.h>
#include <rtems/score/object.h>
#include <rtems/score/objectdata.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsConfigUnitConfig spec:/rtems/config/unit/config
 *
 * @ingroup TestsuitesUnitNoClock0
 *
 * @brief Unit tests for the configuration manager.
 *
 * Parts of the file ``cpukit/sapi/src/getconfigmax.c`` are only executed when
 * unlimited objects are configured.  The space qualified code subset does not
 * support this feature.  This test exercises the code parts otherwise not
 * reached in order to achieve full code coverage.
 *
 * This test case performs the following actions:
 *
 * - Call get_config_max() indirectly through
 *   rtems_configuration_get_maximum_semaphores() with a specially manipulated
 *   argument to enter an if-branch only accessed when unlimited objects are
 *   configured.
 *
 *   - The value returned by the function call must be the one artificially
 *     injected by this test.
 *
 * @{
 */

#define SEMAPHORES_PER_BLOCK 32

/**
 * @brief Test context for spec:/rtems/config/unit/config test case.
 */
typedef struct {
  /**
   * @brief This member contains the original value of
   *   ``_Semaphore_Information->objects_per_block``.
   */
  Objects_Maximum objects_per_block_ori;
} RtemsConfigUnitConfig_Context;

static RtemsConfigUnitConfig_Context
  RtemsConfigUnitConfig_Instance;

static void RtemsConfigUnitConfig_Setup( RtemsConfigUnitConfig_Context *ctx )
{
  ctx->objects_per_block_ori = _Semaphore_Information.objects_per_block;
  _Semaphore_Information.objects_per_block = SEMAPHORES_PER_BLOCK;
}

static void RtemsConfigUnitConfig_Setup_Wrap( void *arg )
{
  RtemsConfigUnitConfig_Context *ctx;

  ctx = arg;
  RtemsConfigUnitConfig_Setup( ctx );
}

static void RtemsConfigUnitConfig_Teardown(
  RtemsConfigUnitConfig_Context *ctx
)
{
  _Semaphore_Information.objects_per_block = ctx->objects_per_block_ori;
}

static void RtemsConfigUnitConfig_Teardown_Wrap( void *arg )
{
  RtemsConfigUnitConfig_Context *ctx;

  ctx = arg;
  RtemsConfigUnitConfig_Teardown( ctx );
}

static T_fixture RtemsConfigUnitConfig_Fixture = {
  .setup = RtemsConfigUnitConfig_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsConfigUnitConfig_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsConfigUnitConfig_Instance
};

/**
 * @brief Call get_config_max() indirectly through
 *   rtems_configuration_get_maximum_semaphores() with a specially manipulated
 *   argument to enter an if-branch only accessed when unlimited objects are
 *   configured.
 */
static void RtemsConfigUnitConfig_Action_0(
  RtemsConfigUnitConfig_Context *ctx
)
{
  uint32_t max = rtems_configuration_get_maximum_semaphores();

  /*
   * The value returned by the function call must be the one artificially
   * injected by this test.
   */
  T_eq_u32( max, SEMAPHORES_PER_BLOCK | RTEMS_UNLIMITED_OBJECTS );
}

/**
 * @fn void T_case_body_RtemsConfigUnitConfig( void )
 */
T_TEST_CASE_FIXTURE( RtemsConfigUnitConfig, &RtemsConfigUnitConfig_Fixture )
{
  RtemsConfigUnitConfig_Context *ctx;

  ctx = T_fixture_context();

  RtemsConfigUnitConfig_Action_0( ctx );
}

/** @} */
