/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSignalValSignals
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
 * @defgroup RtemsSignalValSignals spec:/rtems/signal/val/signals
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief This test case collection provides validation test cases for the @ref
 *   RTEMSAPIClassicSignal.
 *
 * This test case performs the following actions:
 *
 * - Run the signal test for all 32 signal constants.
 *
 * @{
 */

static const rtems_signal_set signals[] = {
  RTEMS_SIGNAL_0,
  RTEMS_SIGNAL_1,
  RTEMS_SIGNAL_2,
  RTEMS_SIGNAL_3,
  RTEMS_SIGNAL_4,
  RTEMS_SIGNAL_5,
  RTEMS_SIGNAL_6,
  RTEMS_SIGNAL_7,
  RTEMS_SIGNAL_8,
  RTEMS_SIGNAL_9,
  RTEMS_SIGNAL_10,
  RTEMS_SIGNAL_11,
  RTEMS_SIGNAL_12,
  RTEMS_SIGNAL_13,
  RTEMS_SIGNAL_14,
  RTEMS_SIGNAL_15,
  RTEMS_SIGNAL_16,
  RTEMS_SIGNAL_17,
  RTEMS_SIGNAL_18,
  RTEMS_SIGNAL_19,
  RTEMS_SIGNAL_20,
  RTEMS_SIGNAL_21,
  RTEMS_SIGNAL_22,
  RTEMS_SIGNAL_23,
  RTEMS_SIGNAL_24,
  RTEMS_SIGNAL_25,
  RTEMS_SIGNAL_26,
  RTEMS_SIGNAL_27,
  RTEMS_SIGNAL_28,
  RTEMS_SIGNAL_29,
  RTEMS_SIGNAL_30,
  RTEMS_SIGNAL_31
};

/**
 * @brief Run the signal test for all 32 signal constants.
 */
static void RtemsSignalValSignals_Action_0( void )
{
  unsigned int i;

  for ( i = 0; i < 32; ++i ) {
    RtemsSignalValSignalConstant_Run( signals[ i ], i );
    T_step( i ); /* Accounts for 32 test plan steps */
  }
}

/**
 * @fn void T_case_body_RtemsSignalValSignals( void )
 */
T_TEST_CASE( RtemsSignalValSignals )
{
  T_plan( 32 );

  RtemsSignalValSignals_Action_0();
}

/** @} */
