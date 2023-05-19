/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIClassicTimecounter
 *
 * @brief This header file provides the Timecounter Support API.
 */

/*
 * Copyright (c) 2015 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_TIMECOUNTER_H
#define _RTEMS_TIMECOUNTER_H

#include <rtems/score/timecounter.h>
#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSAPIClassicTimecounter Timecounter Support
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The timecounter support provides directives to implement clock device
 *   drivers.
 *
 * @{
 */

/**
 * @brief Timecounter quality for the clock drivers.
 *
 * Timecounter with higher quality value are used in favour of those with lower
 * quality value.
 */
#define RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER 100

/**
 * @copydoc _Timecounter_Install()
 *
 * Below is an exemplary code snippet that shows the adjustable parameters and
 * the following call of the install routine.
 *
 * @code
 * struct timecounter tc;
 *
 * uint32_t get_timecount( struct timecounter *tc )
 * {
 *   return some_free_running_counter;
 * }
 *
 * void install( void )
 * {
 *   tc.tc_get_timecount = get_timecount;
 *   tc.tc_counter_mask = 0xffffffff;
 *   tc.tc_frequency = 123456;
 *   tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
 *   rtems_timecounter_install( &tc );
 * }
 * @endcode
 */
static inline void rtems_timecounter_install(
  struct timecounter *tc
)
{
  _Timecounter_Install( tc );
}

/**
 * @copydoc _Timecounter_Tick()
 */
static inline void rtems_timecounter_tick(void)
{
  _Timecounter_Tick();
}

/**
 * @brief Simple timecounter to support legacy clock drivers.
 */
typedef struct {
  struct timecounter tc;
  uint64_t scaler;
  uint32_t real_interval;
  uint32_t binary_interval;
} rtems_timecounter_simple;

/**
 * @brief At tick handling done under protection of the timecounter lock.
 */
typedef void rtems_timecounter_simple_at_tick(
  rtems_timecounter_simple *tc
);

/**
 * @brief Returns the current value of a simple timecounter.
 */
typedef uint32_t rtems_timecounter_simple_get(
  rtems_timecounter_simple *tc
);

/**
 * @brief Returns true if the interrupt of a simple timecounter is pending, and
 * false otherwise.
 */
typedef bool rtems_timecounter_simple_is_pending(
  rtems_timecounter_simple *tc
);

/**
 * @brief Initializes and installs a simple timecounter.
 *
 * A simple timecounter can be used if the hardware provides no free running
 * counter.  A periodic hardware counter must be provided.  The counter period
 * must be synchronous to the clock tick.  The counter ticks per clock tick is
 * scaled up to the next power of two.
 *
 * @param[in] tc Zero initialized simple timecounter.
 * @param[in] counter_frequency_in_hz The hardware counter frequency in Hz.
 * @param[in] counter_ticks_per_clock_tick The hardware counter ticks per clock
 *   tick.
 * @param[in] get_timecount The method to get the current time count.
 *
 * @code
 * #include <rtems/timecounter.h>
 *
 * static rtems_timecounter_simple some_tc;
 *
 * static uint32_t some_tc_get( rtems_timecounter_simple *tc )
 * {
 *   return some.value;
 * }
 *
 * static bool some_tc_is_pending( rtems_timecounter_simple *tc )
 * {
 *   return some.is_pending;
 * }
 *
 * static uint32_t some_tc_get_timecount( struct timecounter *tc )
 * {
 *   return rtems_timecounter_simple_downcounter_get(
 *     tc,
 *     some_tc_get,
 *     some_tc_is_pending
 *   );
 * }
 *
 * static void some_tc_tick( void )
 * {
 *   rtems_timecounter_simple_downcounter_tick( &some_tc, some_tc_get );
 * }
 *
 * void some_tc_init( void )
 * {
 *   uint64_t us_per_tick;
 *   uint32_t counter_frequency_in_hz;
 *   uint32_t counter_ticks_per_clock_tick;
 *
 *   us_per_tick = rtems_configuration_get_microseconds_per_tick();
 *   counter_frequency_in_hz = some_tc_get_frequency();
 *   counter_ticks_per_clock_tick =
 *     (uint32_t) ( counter_frequency_in_hz * us_per_tick ) / 1000000;
 *
 *   some_tc_init_hardware( counter_ticks_per_clock_tick );
 *   some_tc_init_clock_tick_interrupt( some_tc_tick );
 *
 *   rtems_timecounter_simple_install(
 *     &some_tc,
 *     counter_frequency_in_hz,
 *     counter_ticks_per_clock_tick,
 *     some_tc_get_timecount
 *   );
 * }
 * @endcode
 *
 * @see rtems_timecounter_simple_downcounter_get(),
 * rtems_timecounter_simple_downcounter_tick(),
 * rtems_timecounter_simple_upcounter_get() and
 * rtems_timecounter_simple_upcounter_tick().
 */
void rtems_timecounter_simple_install(
  rtems_timecounter_simple *tc,
  uint32_t                  counter_frequency_in_hz,
  uint32_t                  counter_ticks_per_clock_tick,
  timecounter_get_t        *get_timecount
);

/**
 * @brief Maps a simple timecounter value into its binary frequency domain.
 *
 * @param[in] tc The simple timecounter.
 * @param[in] value The value of the simple timecounter.
 *
 * @return The scaled value.
 */
static inline uint32_t rtems_timecounter_simple_scale(
  const rtems_timecounter_simple *tc,
  uint32_t value
)
{
  return (uint32_t) ( ( value * tc->scaler ) >> 32 );
}

/**
 * @brief Performs a simple timecounter tick for downcounters.
 *
 * @param[in] tc The simple timecounter.
 * @param[in] get The method to get the value of the simple timecounter.
 * @param[in] at_tick The method to perform work under timecounter lock
 * protection at this tick, e.g. clear a pending flag.
 */
static inline void rtems_timecounter_simple_downcounter_tick(
  rtems_timecounter_simple         *tc,
  rtems_timecounter_simple_get      get,
  rtems_timecounter_simple_at_tick  at_tick
)
{
  ISR_lock_Context lock_context;
  uint32_t current;

  _Timecounter_Acquire( &lock_context );

  ( *at_tick )( tc );

  current = rtems_timecounter_simple_scale(
    tc,
    tc->real_interval - ( *get )( tc )
  );

  _Timecounter_Tick_simple( tc->binary_interval, current, &lock_context );
}

/**
 * @brief Performs a simple timecounter tick for upcounters.
 *
 * @param[in] tc The simple timecounter.
 * @param[in] get The method to get the value of the simple timecounter.
 * @param[in] at_tick The method to perform work under timecounter lock
 * protection at this tick, e.g. clear a pending flag.
 */
static inline void rtems_timecounter_simple_upcounter_tick(
  rtems_timecounter_simple         *tc,
  rtems_timecounter_simple_get      get,
  rtems_timecounter_simple_at_tick  at_tick
)
{
  ISR_lock_Context lock_context;
  uint32_t current;

  _Timecounter_Acquire( &lock_context );

  ( *at_tick )( tc );

  current = rtems_timecounter_simple_scale( tc, ( *get )( tc ) );

  _Timecounter_Tick_simple( tc->binary_interval, current, &lock_context );
}

/**
 * @brief Gets the simple timecounter value mapped to its binary frequency
 * domain for downcounters.
 *
 * @param[in] tc The simple timecounter.
 * @param[in] get The method to get the value of the simple timecounter.
 * @param[in] is_pending The method which indicates if the interrupt of the
 * simple timecounter is pending.
 */
static inline uint32_t rtems_timecounter_simple_downcounter_get(
  struct timecounter                  *tc_base,
  rtems_timecounter_simple_get         get,
  rtems_timecounter_simple_is_pending  is_pending
)
{
  rtems_timecounter_simple *tc;
  uint32_t counter;
  uint32_t interval;

  tc = (rtems_timecounter_simple *) tc_base;
  counter = ( *get )( tc );
  interval = tc->real_interval;

  if ( ( *is_pending )( tc ) ) {
    counter = ( *get )( tc );
    interval *= 2;
  }

  return rtems_timecounter_simple_scale( tc, interval - counter );
}

/**
 * @brief Gets the simple timecounter value mapped to its binary frequency
 * domain for upcounters.
 *
 * @param[in] tc The simple timecounter.
 * @param[in] get The method to get the value of the simple timecounter.
 * @param[in] is_pending The method which indicates if the interrupt of the
 * simple timecounter is pending.
 */
static inline uint32_t rtems_timecounter_simple_upcounter_get(
  struct timecounter                  *tc_base,
  rtems_timecounter_simple_get         get,
  rtems_timecounter_simple_is_pending  is_pending
)
{
  rtems_timecounter_simple *tc;
  uint32_t counter;
  uint32_t interval;

  tc = (rtems_timecounter_simple *) tc_base;
  counter = ( *get )( tc );
  interval = 0;

  if ( ( *is_pending )( tc ) ) {
    counter = ( *get )( tc );
    interval = tc->real_interval;
  }

  return rtems_timecounter_simple_scale( tc, interval + counter );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_TIMECOUNTER_H */
