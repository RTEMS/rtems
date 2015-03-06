/**
 * @file
 *
 * @ingroup ClassicCounter
 * 
 * @brief Free-Running Counter and Busy Wait Delay API
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_SAPI_COUNTER_H
#define _RTEMS_SAPI_COUNTER_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ClassicCounter Free-Running Counter and Busy Wait Delay
 *
 * @ingroup ClassicRTEMS
 *
 * @brief Free-running counter and busy wait delay functions.
 *
 * The RTEMS counter is some free-running counter.  It ticks usually with a
 * frequency close to the CPU or system bus clock.
 *
 * The counter can be used in case the overhead of the
 * rtems_clock_get_uptime_nanoseconds() is too high.  The step from counter
 * ticks to/from nanoseconds is explicit in this API unlike to
 * rtems_clock_get_uptime_nanoseconds() which performs the conversion on each
 * invocation.
 *
 * This counter works without a clock driver and during system initialization.
 *
 * The counter can be used to profile low-level operations like SMP locks or
 * interrupt disabled critical sections.  The counter can act also as an
 * entropy source for a random number generator. 
 *
 * The period of the counter depends on the actual hardware.
 *
 * @{
 */

/**
 * @brief Unsigned integer type for counter values.
 */
typedef CPU_Counter_ticks rtems_counter_ticks;

/**
 * @brief Reads the current counter values.
 *
 * @return The current counter values.
 */
static inline rtems_counter_ticks rtems_counter_read( void )
{
  return _CPU_Counter_read();
}

/**
 * @brief Returns the difference between the second and first CPU counter
 * value.
 *
 * This operation may be carried out as a modulo operation depending on the
 * range of the CPU counter device.
 *
 * @param[in] second The second CPU counter value.
 * @param[in] first The first CPU counter value.
 *
 * @return Returns second minus first modulo counter period.
 */
static inline rtems_counter_ticks rtems_counter_difference(
  rtems_counter_ticks second,
  rtems_counter_ticks first
)
{
  return _CPU_Counter_difference( second, first );
}

/**
 * @brief Converts counter ticks into nanoseconds.
 *
 * @param[in] ticks Some counter ticks.
 *
 * @return The nanoseconds corresponding to the counter ticks.  The value is
 * rounded up.
 */
uint64_t rtems_counter_ticks_to_nanoseconds(
  rtems_counter_ticks ticks
);

/**
 * @brief Converts nanoseconds into counter ticks.
 *
 * @param[in] nanoseconds Some nanoseconds.
 *
 * @return The counter ticks corresponding to the nanoseconds.  The value is
 * rounded up.
 */
rtems_counter_ticks rtems_counter_nanoseconds_to_ticks(
  uint32_t nanoseconds
);

/**
 * @brief Initializes the counter ticks to/from nanoseconds converter functions.
 *
 * This function must be used to initialize the
 * rtems_counter_ticks_to_nanoseconds() and
 * rtems_counter_nanoseconds_to_ticks() functions.  It should be called during
 * system initialization by the board support package.
 *
 * @param[in] frequency The current counter frequency in Hz.
 */
void rtems_counter_initialize_converter( uint32_t frequency );

/**
 * @brief Busy wait for some counter ticks.
 *
 * This function does not disable interrupts.  Thus task switches and
 * interrupts can interfere with this busy wait may prolong the delay.  This
 * function busy waits at least the specified time.  Due to some overhead the
 * actual delay may be longer.
 *
 * @param[in] ticks The minimum busy wait time in counter ticks.
 */
void rtems_counter_delay_ticks( rtems_counter_ticks ticks );

/**
 * @brief Busy wait for some nanoseconds.
 *
 * This function does not disable interrupts.  Thus task switches and
 * interrupts can interfere with this busy wait may prolong the delay.  This
 * function busy waits at least the specified time.  Due to some overhead the
 * actual delay may be longer.
 *
 * @param[in] nanoseconds The minimum busy wait time in nanoseconds.
 */
void rtems_counter_delay_nanoseconds( uint32_t nanoseconds );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SAPI_COUNTER_H */
