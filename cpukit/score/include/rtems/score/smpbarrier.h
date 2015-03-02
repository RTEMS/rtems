/**
 * @file
 *
 * @ingroup ScoreSMPBarrier
 *
 * @brief SMP Barrier API
 */

/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SMPBARRIER_H
#define _RTEMS_SCORE_SMPBARRIER_H

#include <rtems/score/cpuopts.h>
#include <rtems/score/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreSMPBarrier SMP Barriers
 *
 * @ingroup Score
 *
 * @brief The SMP barrier provides barrier synchronization for SMP systems at
 * the lowest level.
 *
 * The SMP barrier is implemented as a sense barrier, see also Herlihy and
 * Shavit, "The Art of Multiprocessor Programming", 17.3 Sense-Reversing
 * Barrier.
 *
 * @{
 */

/**
 * @brief SMP barrier control.
 */
typedef struct {
  Atomic_Uint value;
  Atomic_Uint sense;
} SMP_barrier_Control;

/**
 * @brief SMP barrier per-thread state.
 *
 * Each user of the barrier must provide this per-thread state.
 */
typedef struct {
  unsigned int sense;
} SMP_barrier_State;

/**
 * @brief SMP barrier control initializer for static initialization.
 */
#define SMP_BARRIER_CONTROL_INITIALIZER \
  { ATOMIC_INITIALIZER_UINT( 0U ), ATOMIC_INITIALIZER_UINT( 0U ) }

/**
 * @brief SMP barrier per-thread state initializer for static initialization.
 */
#define SMP_BARRIER_STATE_INITIALIZER { 0U }

/**
 * @brief Initializes a SMP barrier control.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[out] control The SMP barrier control.
 */
static inline void _SMP_barrier_Control_initialize(
  SMP_barrier_Control *control
)
{
  _Atomic_Init_uint( &control->value, 0U );
  _Atomic_Init_uint( &control->sense, 0U );
}

/**
 * @brief Initializes a SMP barrier per-thread state.
 *
 * @param[out] state The SMP barrier control.
 */
static inline void _SMP_barrier_State_initialize(
  SMP_barrier_State *state
)
{
  state->sense = 0U;
}

/**
 * @brief Waits on the SMP barrier until count threads rendezvoused.
 *
 * @param[in, out] control The SMP barrier control.
 * @param[in, out] state The SMP barrier per-thread state.
 * @param[in] count The thread count bound to rendezvous.
 */
void _SMP_barrier_Wait(
  SMP_barrier_Control *control,
  SMP_barrier_State *state,
  unsigned int count
);

/**@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SMPBARRIER_H */
