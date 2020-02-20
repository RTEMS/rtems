/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate Clock Configuration Options
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_CONFDEFS_CLOCK_H
#define _RTEMS_CONFDEFS_CLOCK_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#include <rtems/score/watchdogticks.h>

#if !defined(CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER) \
  && !defined(CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER) \
  && !defined(CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER)
  #error "You must define one of CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER, CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER, and CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER"
#endif

#if ( defined(CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER) \
  + defined(CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER) + \
  + defined(CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER) ) > 1
  #error "CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER, CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER, and CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER are mutually exclusive"
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
  #include <rtems/clockdrv.h>
  #include <rtems/sysinit.h>
#endif

#ifndef CONFIGURE_MICROSECONDS_PER_TICK
  #define CONFIGURE_MICROSECONDS_PER_TICK 10000
#endif

#if 1000000 % CONFIGURE_MICROSECONDS_PER_TICK != 0
  #warning "The clock ticks per second is not an integer"
#endif

#if CONFIGURE_MICROSECONDS_PER_TICK <= 0
  #error "CONFIGURE_MICROSECONDS_PER_TICK must be positive"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
  RTEMS_SYSINIT_ITEM(
    _Clock_Initialize,
    RTEMS_SYSINIT_DEVICE_DRIVERS,
    RTEMS_SYSINIT_ORDER_THIRD
  );
#endif

const uint32_t _Watchdog_Microseconds_per_tick =
  CONFIGURE_MICROSECONDS_PER_TICK;

const uint32_t _Watchdog_Nanoseconds_per_tick =
  (uint32_t) 1000 * CONFIGURE_MICROSECONDS_PER_TICK;

const uint32_t _Watchdog_Ticks_per_second =
  1000000 / CONFIGURE_MICROSECONDS_PER_TICK;

#if defined(CONFIGURE_TICKS_PER_TIMESLICE) \
  && CONFIGURE_TICKS_PER_TIMESLICE != WATCHDOG_TICKS_PER_TIMESLICE_DEFAULT
  const uint32_t _Watchdog_Ticks_per_timeslice =
    CONFIGURE_TICKS_PER_TIMESLICE;
#endif

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_CLOCK_H */
