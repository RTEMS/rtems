/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSysState
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreSysState.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_SCORE_SYSSTATE_H
#define _RTEMS_SCORE_SYSSTATE_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreSysState System State Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the System State Handler implementation.
 *
 * @{
 */

/**
 * @brief System states.
 */
typedef enum {
  /**
   * @brief The system is before the end of the first phase of initialization.
   */
  SYSTEM_STATE_BEFORE_INITIALIZATION,

  /**
   * @brief The system is between end of the first phase of initialization but
   * before  multitasking is started.
   */
  SYSTEM_STATE_BEFORE_MULTITASKING,

  /**
   * @brief The system is up and operating normally.
   */
  SYSTEM_STATE_UP,

  /**
   * @brief The system reached its terminal state.
   */
  SYSTEM_STATE_TERMINATED
} System_state_Codes;

#define SYSTEM_STATE_CODES_FIRST SYSTEM_STATE_BEFORE_INITIALIZATION

#define SYSTEM_STATE_CODES_LAST SYSTEM_STATE_TERMINATED

#if defined(RTEMS_MULTIPROCESSING)
extern bool _System_state_Is_multiprocessing;
#endif

extern System_state_Codes _System_state_Current;

/**
 * @brief Sets the current system state to the given state.
 *
 * @param state The state to set.
 */
static inline void _System_state_Set (
  System_state_Codes state
)
{
  _System_state_Current = state;
}

/**
 * @brief Gets the current system state.
 *
 * @return The current system state.
 */
static inline System_state_Codes _System_state_Get ( void )
{
  return _System_state_Current;
}

/**
 * @brief Checks if the state is before initialization.
 *
 * @param state The state to check.
 *
 * @retval true @a state is before initialization.
 * @retval false @a state is not before initialization.
 */
static inline bool _System_state_Is_before_initialization (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_INITIALIZATION);
}

/**
 * @brief Checks if the state is before multitasking.
 *
 * @param state The state to check.
 *
 * @retval true @a state is before multitasking.
 * @retval false @a state is not before multitasking.
 */
static inline bool _System_state_Is_before_multitasking (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_MULTITASKING);
}

/**
 * @brief Checks if the state is up.
 *
 * @param state The state to check.
 *
 * @retval true @a state is up.
 * @retval false @a state is not up.
 */
static inline bool _System_state_Is_up (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_UP);
}

/**
 * @brief Checks if the state is terminated.
 *
 * @param state The state to check.
 *
 * @retval true @a state is terminated.
 * @retval false @a state is not terminated.
 */
static inline bool _System_state_Is_terminated (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_TERMINATED);
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
