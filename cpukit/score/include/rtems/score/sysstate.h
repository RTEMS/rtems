/**
 * @file
 *
 * @ingroup ScoreSysState
 *
 * @brief System State Handler API
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SYSSTATE_H
#define _RTEMS_SCORE_SYSSTATE_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ScoreSysState System State Handler
 *
 * @ingroup Score
 *
 * @brief Management of the internal system state of RTEMS.
 */
/**@{**/

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
SCORE_EXTERN bool _System_state_Is_multiprocessing;
#endif

extern System_state_Codes _System_state_Current;

RTEMS_INLINE_ROUTINE void _System_state_Set (
  System_state_Codes state
)
{
  _System_state_Current = state;
}

RTEMS_INLINE_ROUTINE void _System_state_Handler_initialization (
#if defined(RTEMS_MULTIPROCESSING)
  bool  is_multiprocessing
#else
  bool  is_multiprocessing __attribute__((unused))
#endif
)
{
  _System_state_Set( SYSTEM_STATE_BEFORE_INITIALIZATION );
#if defined(RTEMS_MULTIPROCESSING)
    _System_state_Is_multiprocessing = is_multiprocessing;
#endif
}

RTEMS_INLINE_ROUTINE System_state_Codes _System_state_Get ( void )
{
  return _System_state_Current;
}

RTEMS_INLINE_ROUTINE bool _System_state_Is_before_initialization (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_INITIALIZATION);
}

RTEMS_INLINE_ROUTINE bool _System_state_Is_before_multitasking (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_BEFORE_MULTITASKING);
}

RTEMS_INLINE_ROUTINE bool _System_state_Is_up (
  System_state_Codes state
)
{
  return (state == SYSTEM_STATE_UP);
}

RTEMS_INLINE_ROUTINE bool _System_state_Is_terminated (
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
