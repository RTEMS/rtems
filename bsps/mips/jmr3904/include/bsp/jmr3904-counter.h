/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMIPSJMR3904
 *
 * @brief This header file provides the interfaces of the free running counter.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_MIPS_JMR3904_COUNTER_H
#define LIBBSP_MIPS_JMR3904_COUNTER_H

#include <stdint.h>

#include <rtems/config.h>

#include <libcpu/tx3904.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSBSPsMIPSJMR3904
 *
 * @{
 */

/**
 * @brief This constant defines the clicks of the clock tick timer per clock
 *   tick.
 *
 * The value is arbitrary.  It defines the frequency of the timers of this BSP
 * in terms of the clock tick, because they all run with the same divider.
 */
#define JMR3904_TIMER_CLICKS_PER_TICK 5000

/**
 * @brief This constant defines the frequency of the free running counter in
 *   Hz.
 */
#define JMR3904_COUNTER_FREQUENCY \
  ( (uint32_t) ( ( UINT64_C( JMR3904_TIMER_CLICKS_PER_TICK ) * 1000000 ) / \
                 rtems_configuration_get_microseconds_per_tick() ) )

/**
 * @brief This constant defines the timer which provides the free running
 *   counter.
 *
 * Timer 0 is the clock tick.
 */
#define JMR3904_COUNTER_BASE TX3904_TIMER1_BASE

/**
 * @brief This constant defines the width of the counter of a timer.
 */
#define JMR3904_COUNTER_MASK 0xffffff

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_MIPS_JMR3904_COUNTER_H */
