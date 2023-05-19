/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSMPLock
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreSMPLock related to lock statistics.
 */

/*
 * Copyright (C) 2013, 2018 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_SMPLOCKSTATS_H
#define _RTEMS_SCORE_SMPLOCKSTATS_H

#include <rtems/score/cpu.h>

#if defined(RTEMS_SMP)

#include <rtems/score/chain.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSScoreSMPLock
 *
 * @{
 */

#if defined(RTEMS_PROFILING)

/**
 * @brief Count of lock contention counters for lock statistics.
 */
#define SMP_LOCK_STATS_CONTENTION_COUNTS 4

/**
 * @brief SMP lock statistics.
 *
 * The lock acquire attempt instant is the point in time right after the
 * interrupt disable action in the lock acquire sequence.
 *
 * The lock acquire instant is the point in time right after the lock
 * acquisition.  This is the begin of the critical section code execution.
 *
 * The lock release instant is the point in time right before the interrupt
 * enable action in the lock release sequence.
 *
 * The lock section time is the time elapsed between the lock acquire instant
 * and the lock release instant.
 *
 * The lock acquire time is the time elapsed between the lock acquire attempt
 * instant and the lock acquire instant.
 */
typedef struct {
  /**
   * @brief Node for SMP lock statistics chain.
   */
  Chain_Node Node;

  /**
   * @brief The maximum lock acquire time in CPU counter ticks.
   */
  CPU_Counter_ticks max_acquire_time;

  /**
   * @brief The maximum lock section time in CPU counter ticks.
   */
  CPU_Counter_ticks max_section_time;

  /**
   * @brief The count of lock uses.
   *
   * This value may overflow.
   */
  uint64_t usage_count;

  /**
   * @brief Total lock acquire time in nanoseconds.
   *
   * The average lock acquire time is the total acquire time divided by the
   * lock usage count.  The ration of the total section and total acquire times
   * gives a measure for the lock contention.
   *
   * This value may overflow.
   */
  uint64_t total_acquire_time;

  /**
   * @brief The counts of lock acquire operations by contention.
   *
   * The contention count for index N corresponds to a lock acquire attempt
   * with an initial queue length of N.  The last index corresponds to all
   * lock acquire attempts with an initial queue length greater than or equal
   * to SMP_LOCK_STATS_CONTENTION_COUNTS minus one.
   *
   * The values may overflow.
   */
  uint64_t contention_counts[SMP_LOCK_STATS_CONTENTION_COUNTS];

  /**
   * @brief Total lock section time in CPU counter ticks.
   *
   * The average lock section time is the total section time divided by the
   * lock usage count.
   *
   * This value may overflow.
   */
  uint64_t total_section_time;

  /**
   * @brief The lock name.
   */
  const char *name;
} SMP_lock_Stats;

/**
 * @brief Local context for SMP lock statistics.
 */
typedef struct {
  /**
   * @brief The last lock acquire instant in CPU counter ticks.
   *
   * This value is used to measure the lock section time.
   */
  CPU_Counter_ticks acquire_instant;

  /**
   * @brief The lock stats used for the last lock acquire.
   */
  SMP_lock_Stats *stats;
} SMP_lock_Stats_context;

/**
 * @brief SMP lock statistics initializer for static initialization.
 */
#define SMP_LOCK_STATS_INITIALIZER( name ) \
  { { NULL, NULL }, 0, 0, 0, 0, { 0, 0, 0, 0 }, 0, name }

/**
 * @brief Initializes an SMP lock statistics block.
 *
 * @param[in, out] stats The SMP lock statistics block.
 * @param name The name for the SMP lock statistics.  This name must be
 * persistent throughout the life time of this statistics block.
 */
static inline void _SMP_lock_Stats_initialize(
  SMP_lock_Stats *stats,
  const char *name
)
{
  SMP_lock_Stats init = SMP_LOCK_STATS_INITIALIZER( name );

  *stats = init;
}

/**
 * @brief Destroys an SMP lock statistics block.
 *
 * @param[out] stats The SMP lock statistics block.
 */
void _SMP_lock_Stats_destroy( SMP_lock_Stats *stats );

/**
 * @brief Sets the max section time of the SMP lock statistics block and
 *      registers it.
 *
 * @param[in, out] stats The SMP lock statistics block.
 * @param max_section_time The max section time for @a stats.
 */
void _SMP_lock_Stats_register_or_max_section_time(
  SMP_lock_Stats    *stats,
  CPU_Counter_ticks  max_section_time
);

typedef struct {
  CPU_Counter_ticks first;
} SMP_lock_Stats_acquire_context;

/**
 * @brief Starts the lock stats for acquire.
 *
 * @param[in, out] acquire_context The acquire context.
 */
static inline void _SMP_lock_Stats_acquire_begin(
  SMP_lock_Stats_acquire_context *acquire_context
)
{
  acquire_context->first = _CPU_Counter_read();
}

/**
 * @brief Ends the lock stats for acquire.
 *
 * @param acquire_context The acquire context for the operation.
 * @param[in, out] stats The stats to modify.
 * @param[out] stats_context The context for the stats.
 * @param queue_length The queue length for the stats contention counts.
 */
static inline void _SMP_lock_Stats_acquire_end(
  const SMP_lock_Stats_acquire_context *acquire_context,
  SMP_lock_Stats                       *stats,
  SMP_lock_Stats_context               *stats_context,
  unsigned int                          queue_length
)
{
  CPU_Counter_ticks second;
  CPU_Counter_ticks delta;

  second = _CPU_Counter_read();
  stats_context->acquire_instant = second;
  delta = second - acquire_context->first;

  ++stats->usage_count;

  stats->total_acquire_time += delta;

  if ( stats->max_acquire_time < delta ) {
    stats->max_acquire_time = delta;
  }

  if ( queue_length >= SMP_LOCK_STATS_CONTENTION_COUNTS ) {
    queue_length = SMP_LOCK_STATS_CONTENTION_COUNTS - 1;
  }
  ++stats->contention_counts[ queue_length ];

  stats_context->stats = stats;
}

/**
 * @brief Updates an SMP lock statistics block during a lock release.
 *
 * @param[in, out] stats_context The SMP lock statistics context.
 */
static inline void _SMP_lock_Stats_release_update(
  const SMP_lock_Stats_context *stats_context
)
{
  SMP_lock_Stats    *stats;
  CPU_Counter_ticks  first;
  CPU_Counter_ticks  second;
  CPU_Counter_ticks  delta;

  stats = stats_context->stats;
  first = stats_context->acquire_instant;
  second = _CPU_Counter_read();
  delta = second - first;

  stats->total_section_time += delta;

  if ( stats->max_section_time < delta ) {
    _SMP_lock_Stats_register_or_max_section_time( stats, delta );
  }
}

typedef struct {
  Chain_Node Node;
  SMP_lock_Stats *current;
} SMP_lock_Stats_iteration_context;

/**
 * @brief Adds a newly initialized node to the iteration context.
 *
 * @param[in, out] iteration_context The iteration context for the operation.
 */
void _SMP_lock_Stats_iteration_start(
  SMP_lock_Stats_iteration_context *iteration_context
);

/**
 * @brief Gets the next statistics node, if it is valid.
 *
 * @param[in, out] iteration_context The iteration context.
 * @param[out] snapshot Contains the next node of the stats chain.
 * @param[in, out] name The name for @a snapshot.
 * @param name_size The size of @a name.
 *
 * @retval true The node is valid.
 * @retval false The node is not valid.
 */
bool _SMP_lock_Stats_iteration_next(
  SMP_lock_Stats_iteration_context *iteration_context,
  SMP_lock_Stats                   *snapshot,
  char                             *name,
  size_t                            name_size
);

/**
 * @brief Removes a node from the iteration context.
 *
 * @param[in, out] iteration_context The iteration context for the operation.
 */
void _SMP_lock_Stats_iteration_stop(
  SMP_lock_Stats_iteration_context *iteration_context
);

#else /* RTEMS_PROFILING */

#define _SMP_lock_Stats_initialize( stats, name ) do { } while ( 0 )

#define _SMP_lock_Stats_destroy( stats ) do { } while ( 0 )

#endif /* !RTEMS_PROFILING */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_SMP */

#endif /* _RTEMS_SCORE_SMPLOCKSTATS_H */
