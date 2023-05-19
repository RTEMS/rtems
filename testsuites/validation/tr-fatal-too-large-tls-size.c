/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValFatalTooLargeTlsSize
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
#include <rtems/score/thread.h>

#include "tr-fatal-too-large-tls-size.h"

#include <rtems/test.h>

/**
 * @defgroup AcfgValFatalTooLargeTlsSize \
 *   spec:/acfg/val/fatal-too-large-tls-size
 *
 * @ingroup TestsuitesFatalTooLargeTlsSize
 *
 * @brief Tests a fatal error.
 *
 * This test case performs the following actions:
 *
 * - The test action is carried out by providing a thread-local storage object
 *   of sufficient size with respect to the application configuration of the
 *   test suite.
 *
 *   - Check that the expected fatal source is present.
 *
 *   - Check that the expected fatal code is present.
 *
 *   - Check that the CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE application
 *     configuration option resulted in the expected system setting.
 *
 * @{
 */

/**
 * @brief Test context for spec:/acfg/val/fatal-too-large-tls-size test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   AcfgValFatalTooLargeTlsSize_Run() parameter.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains a copy of the corresponding
   *   AcfgValFatalTooLargeTlsSize_Run() parameter.
   */
  rtems_fatal_code code;
} AcfgValFatalTooLargeTlsSize_Context;

static AcfgValFatalTooLargeTlsSize_Context
  AcfgValFatalTooLargeTlsSize_Instance;

static volatile _Thread_local uint8_t large_tls_object[ RTEMS_TASK_STORAGE_ALIGNMENT + 1 ];

static volatile bool shall_not_load_the_value;

static T_fixture AcfgValFatalTooLargeTlsSize_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &AcfgValFatalTooLargeTlsSize_Instance
};

/**
 * @brief The test action is carried out by providing a thread-local storage
 *   object of sufficient size with respect to the application configuration of
 *   the test suite.
 */
static void AcfgValFatalTooLargeTlsSize_Action_0(
  AcfgValFatalTooLargeTlsSize_Context *ctx
)
{
  if ( shall_not_load_the_value ) {
    uint8_t value;

    value = large_tls_object[ 0 ];
    RTEMS_OBFUSCATE_VARIABLE( value );
  }

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
    INTERNAL_ERROR_TOO_LARGE_TLS_SIZE
  );

  /*
   * Check that the CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE application
   * configuration option resulted in the expected system setting.
   */
  T_step_eq_sz(
    2,
    _Thread_Maximum_TLS_size,
    RTEMS_TASK_STORAGE_ALIGNMENT
  );
}

void AcfgValFatalTooLargeTlsSize_Run(
  rtems_fatal_source source,
  rtems_fatal_code   code
)
{
  AcfgValFatalTooLargeTlsSize_Context *ctx;

  ctx = &AcfgValFatalTooLargeTlsSize_Instance;
  ctx->source = source;
  ctx->code = code;

  ctx = T_case_begin(
    "AcfgValFatalTooLargeTlsSize",
    &AcfgValFatalTooLargeTlsSize_Fixture
  );

  T_plan( 3 );

  AcfgValFatalTooLargeTlsSize_Action_0( ctx );

  T_case_end();
}

/** @} */
