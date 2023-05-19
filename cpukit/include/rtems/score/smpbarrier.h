/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSMPBarrier
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreSMPBarrier.
 */

/*
 * Copyright (C) 2013, 2014 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_SMPBARRIER_H
#define _RTEMS_SCORE_SMPBARRIER_H

#include <rtems/score/cpuopts.h>
#include <rtems/score/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreSMPBarrier SMP Barriers
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the SMP barrier implementation.
 *
 * The SMP barrier provides barrier synchronization for SMP systems at the
 * lowest level.
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
 * @param count The thread count bound to rendezvous.
 *
 * @retval true This processor performed the barrier release.
 * @retval false This processor did not performe the barrier release.
 */
bool _SMP_barrier_Wait(
  SMP_barrier_Control *control,
  SMP_barrier_State *state,
  unsigned int count
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SMPBARRIER_H */
