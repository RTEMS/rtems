/**
 *  @file  rtems/score/states.h
 *
 *  @brief Thread Execution State Information
 *
 *  This include file defines thread execution state information.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_STATES_H
#define _RTEMS_SCORE_STATES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreStates SuperCore Thread States
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which relates to the management of
 *  the state bitmap associated with each thread.
 */
/**@{*/

/**
 *  The following type defines the control block used to manage a
 *  thread's state.
 */
typedef uint32_t   States_Control;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
