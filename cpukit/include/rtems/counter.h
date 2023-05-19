/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPICounter
 * 
 * @brief This header file defines the Free-Running Counter and Busy Wait Delay
 *   API.
 */

/*
 * Copyright (C) 2014, 2018 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SAPI_COUNTER_H
#define _RTEMS_SAPI_COUNTER_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSAPICounter Free-Running Counter and Busy Wait Delay
 *
 * @ingroup RTEMSAPI
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
 * @brief Returns the current counter frequency in Hz.
 *
 * @return The current counter frequency in Hz.
 */
static inline uint32_t rtems_counter_frequency( void )
{
  return _CPU_Counter_frequency();
}

/**
 * @brief Reads the current counter value.
 *
 * @return The current counter value.
 */
static inline rtems_counter_ticks rtems_counter_read( void )
{
  return _CPU_Counter_read();
}

/**
 * @brief Returns the difference between the second and first CPU counter
 * value.
 *
 * This function is provided for backward compatibility.
 * You may use "second - first" directly in the code.
 *
 * @param[in] second The second CPU counter value.
 * @param[in] first The first CPU counter value.
 *
 * @return Returns second minus first.
 */
static inline rtems_counter_ticks rtems_counter_difference(
  rtems_counter_ticks second,
  rtems_counter_ticks first
)
{
  return second - first;
}

/**
 * @brief Converts counter ticks into nanoseconds.
 *
 * @param[in] ticks The counter ticks value to convert.
 *
 * @return The nanoseconds value corresponding to the counter ticks.  The value
 * is rounded up.
 */
uint64_t rtems_counter_ticks_to_nanoseconds(
  rtems_counter_ticks ticks
);

/**
 * @brief Converts nanoseconds into counter ticks.
 *
 * @param[in] nanoseconds The nanoseconds value to convert.
 *
 * @return The counter ticks value corresponding to the nanoseconds value.  The
 * value is rounded up.
 */
rtems_counter_ticks rtems_counter_nanoseconds_to_ticks(
  uint32_t nanoseconds
);

/**
 * @brief Converts counter ticks into signed binary time (sbintime_t).
 *
 * @param[in] ticks The counter ticks value to convert.
 *
 * @return The signed binary time value corresponding to the counter ticks
 * value.  The value is rounded up.
 */
int64_t rtems_counter_ticks_to_sbintime( rtems_counter_ticks ticks );

/**
 * @brief Converts signed binary time (sbintime_t) into counter ticks.
 *
 * @param[in] sbt The signed binary time value to convert.
 *
 * @return The counter ticks value corresponding to the nanoseconds value.  The
 * value is rounded up.
 */
rtems_counter_ticks rtems_counter_sbintime_to_ticks( int64_t sbt );

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
