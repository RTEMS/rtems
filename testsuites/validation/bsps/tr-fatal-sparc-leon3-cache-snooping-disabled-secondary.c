/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary
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

#include <bsp/bootcard.h>
#include <bsp/fatal.h>
#include <bsp/leon3.h>

#include "tr-fatal-sparc-leon3-cache-snooping-disabled-secondary.h"

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary \
 *   spec:/bsp/sparc/leon3/val/fatal-cache-snooping-disabled-secondary
 *
 * @ingroup TestsuitesBspsFatalSparcLeon3CacheSnoopingDisabledSecondary
 *
 * @brief Tests a fatal error.
 *
 * This test case performs the following actions:
 *
 * - The test action is carried out by the wrapped
 *   bsp_start_on_secondary_processor() function.
 *
 *   - Check that the expected fatal source is present.
 *
 *   - Check that the expected fatal code is present.
 *
 * @{
 */

/**
 * @brief Test context for
 *   spec:/bsp/sparc/leon3/val/fatal-cache-snooping-disabled-secondary test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Run() parameter.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains a copy of the corresponding
   *   BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Run() parameter.
   */
  rtems_fatal_code code;
} BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Context;

static BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Context
  BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Instance;

void __real_bsp_start_on_secondary_processor( struct Per_CPU_Control *cpu_self );

void __wrap_bsp_start_on_secondary_processor( struct Per_CPU_Control *cpu_self );

void __wrap_bsp_start_on_secondary_processor( struct Per_CPU_Control *cpu_self )
{
  uint32_t control;

  control = leon3_get_cache_control_register();
  control &= ~LEON3_REG_CACHE_CTRL_DS;
  leon3_set_cache_control_register( control );

  __real_bsp_start_on_secondary_processor( cpu_self );
}

static T_fixture BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Instance
};

/**
 * @brief The test action is carried out by the wrapped
 *   bsp_start_on_secondary_processor() function.
 */
static void BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Action_0(
  BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Context *ctx
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
    LEON3_FATAL_INVALID_CACHE_CONFIG_SECONDARY_PROCESSOR
  );
}

void BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Run(
  rtems_fatal_source source,
  rtems_fatal_code   code
)
{
  BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Context *ctx;

  ctx = &BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Instance;
  ctx->source = source;
  ctx->code = code;

  ctx = T_case_begin(
    "BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary",
    &BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Fixture
  );

  T_plan( 2 );

  BspSparcLeon3ValFatalCacheSnoopingDisabledSecondary_Action_0( ctx );

  T_case_end();
}

/** @} */
