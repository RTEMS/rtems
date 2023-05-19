/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValDisabledBspSettings
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

#include <bsp.h>
#include <rtems/score/userextdata.h>

#include <rtems/test.h>

/**
 * @defgroup AcfgValDisabledBspSettings spec:/acfg/val/disabled-bsp-settings
 *
 * @ingroup TestsuitesValidationAcfg1
 *
 * @brief Tests the default values of application configuration options where
 *   all optional BSP provided settings are disabled.
 *
 * This test case performs the following actions:
 *
 * - Check the effect of application configuration options with optional
 *   BSP-provided settings.
 *
 *   - Check the default value CONFIGURE_IDLE_TASK_BODY where the optional
 *     BSP-provided default value is disabled.
 *
 *   - Check the default value CONFIGURE_IDLE_TASK_STACK_SIZE where the
 *     optional BSP-provided default value is disabled.
 *
 *   - Check the default value CONFIGURE_INTERRUPT_STACK_SIZE where the
 *     optional BSP-provided default value is disabled.
 *
 *   - Check the BSP-provided initial extension is not registered.
 *
 * @{
 */

/**
 * @brief Check the effect of application configuration options with optional
 *   BSP-provided settings.
 */
static void AcfgValDisabledBspSettings_Action_0( void )
{
  rtems_extensions_table bsp = BSP_INITIAL_EXTENSION;

  /*
   * Check the default value CONFIGURE_IDLE_TASK_BODY where the optional
   * BSP-provided default value is disabled.
   */
  T_eq_ptr(
    rtems_configuration_get_idle_task(),
    _CPU_Thread_Idle_body
  );

  /*
   * Check the default value CONFIGURE_IDLE_TASK_STACK_SIZE where the optional
   * BSP-provided default value is disabled.
   */
  T_eq_sz(
    rtems_configuration_get_idle_task_stack_size(),
    CPU_STACK_MINIMUM_SIZE
  );

  /*
   * Check the default value CONFIGURE_INTERRUPT_STACK_SIZE where the optional
   * BSP-provided default value is disabled.
   */
  T_eq_sz(
    rtems_configuration_get_interrupt_stack_size(),
    CPU_STACK_MINIMUM_SIZE
  );

  /*
   * Check the BSP-provided initial extension is not registered.
   */
  T_eq_sz( _User_extensions_Initial_count, 1 );
  T_ne_ptr(
    _User_extensions_Initial_extensions[ 0 ].fatal,
    bsp.fatal
  );
}

/**
 * @fn void T_case_body_AcfgValDisabledBspSettings( void )
 */
T_TEST_CASE( AcfgValDisabledBspSettings )
{
  AcfgValDisabledBspSettings_Action_0();
}

/** @} */
