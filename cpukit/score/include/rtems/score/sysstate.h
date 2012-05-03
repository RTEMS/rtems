/**
 * @file
 *
 * @ingroup ScoreSysState
 *
 * @brief System State Handler API.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SYSSTATE_H
#define _RTEMS_SCORE_SYSSTATE_H

#include <rtems/score/basedefs.h> /* SCORE_EXTERN */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ScoreSysState System State Handler
 *
 * @ingroup Score
 *
 * @brief Management of the internal system state of RTEMS.
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
   * @brief The system is attempting to initiate multitasking.
   */
  SYSTEM_STATE_BEGIN_MULTITASKING,

  /**
   * @brief The system is up and operating normally.
   */
  SYSTEM_STATE_UP,

  /**
   * @brief The system is in the midst of a shutdown.
   */
  SYSTEM_STATE_SHUTDOWN,

  /**
   * @brief A fatal error has occurred.
   */
  SYSTEM_STATE_FAILED
} System_state_Codes;

#define SYSTEM_STATE_CODES_FIRST SYSTEM_STATE_BEFORE_INITIALIZATION

#define SYSTEM_STATE_CODES_LAST SYSTEM_STATE_FAILED

#if defined(RTEMS_MULTIPROCESSING)
SCORE_EXTERN bool _System_state_Is_multiprocessing;
#endif

SCORE_EXTERN System_state_Codes _System_state_Current;

/*
 *  Make it possible for the application to get the system state information.
 */

#include <rtems/score/sysstate.inl>

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
