/**
 * @file
 *
 * @ingroup SAPITimecounter
 *
 * @brief Timecounter API
 */

/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_TIMECOUNTER_H
#define _RTEMS_TIMECOUNTER_H

#include <rtems/score/timecounter.h>
#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup SAPITimecounter Timecounter Support
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
RTEMS_INLINE_ROUTINE void rtems_timecounter_install(
  struct timecounter *tc
)
{
  _Timecounter_Install( tc );
}

/**
 * @copydoc _Timecounter_Tick()
 */
RTEMS_INLINE_ROUTINE void rtems_timecounter_tick(void)
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
 * counter or only the module used for the clock tick is available.  The period
 * of the simple timecounter equals the clock tick interval.  The interval is
 * scaled up to the next power of two.
 *
 * @param[in] tc Zero initialized simple timecounter.
 * @param[in] frequency_in_hz The timecounter frequency in Hz.
 * @param[in] timecounter_ticks_per_clock_tick The timecounter ticks per clock tick.
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
 * void install( void )
 * {
 *   uint32_t frequency = 123456;
 *   uint64_t us_per_tick = rtems_configuration_get_microseconds_per_tick();
 *   uint32_t timecounter_ticks_per_clock_tick =
 *     ( frequency * us_per_tick ) / 1000000;
 *
 *   rtems_timecounter_simple_install(
 *     &some_tc,
 *     frequency,
 *     timecounter_ticks_per_clock_tick,
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
  uint32_t                  frequency_in_hz,
  uint32_t                  timecounter_ticks_per_clock_tick,
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
RTEMS_INLINE_ROUTINE uint32_t rtems_timecounter_simple_scale(
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
RTEMS_INLINE_ROUTINE void rtems_timecounter_simple_downcounter_tick(
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
RTEMS_INLINE_ROUTINE void rtems_timecounter_simple_upcounter_tick(
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
RTEMS_INLINE_ROUTINE uint32_t rtems_timecounter_simple_downcounter_get(
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
RTEMS_INLINE_ROUTINE uint32_t rtems_timecounter_simple_upcounter_get(
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
