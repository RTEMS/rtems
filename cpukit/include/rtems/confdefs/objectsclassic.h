/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate Classic API Objects Configuration Options
 *
 * For the task objects configuration see <rtems/confdefs/threads.h>.
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

#ifndef _RTEMS_CONFDEFS_OBJECTSCLASSIC_H
#define _RTEMS_CONFDEFS_OBJECTSCLASSIC_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#include <rtems/confdefs/unlimited.h>

#if CONFIGURE_MAXIMUM_BARRIERS > 0
  #include <rtems/rtems/barrierdata.h>
#endif

#if CONFIGURE_MAXIMUM_PORTS > 0
  #include <rtems/rtems/dpmemdata.h>
#endif

#if CONFIGURE_MAXIMUM_MESSAGE_QUEUES > 0
  #include <rtems/rtems/messagedata.h>
#endif

#if CONFIGURE_MAXIMUM_PARTITIONS > 0
  #include <rtems/rtems/partdata.h>
#endif

#if CONFIGURE_MAXIMUM_PERIODS > 0
  #include <rtems/rtems/ratemondata.h>
#endif

#if CONFIGURE_MAXIMUM_REGIONS > 0
  #include <rtems/rtems/regiondata.h>
#endif

#if CONFIGURE_MAXIMUM_SEMAPHORES > 0
  #include <rtems/confdefs/scheduler.h>
  #include <rtems/rtems/semdata.h>
#endif

#if CONFIGURE_MAXIMUM_TIMERS > 0
  #include <rtems/rtems/timerdata.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIGURE_MAXIMUM_BARRIERS > 0
  BARRIER_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_BARRIERS );
#endif

#if CONFIGURE_MAXIMUM_MESSAGE_QUEUES > 0
  MESSAGE_QUEUE_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_MESSAGE_QUEUES );
#endif

#if CONFIGURE_MAXIMUM_PARTITIONS > 0
  PARTITION_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_PARTITIONS );
#endif

#if CONFIGURE_MAXIMUM_PERIODS > 0
  RATE_MONOTONIC_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_PERIODS );
#endif

#if CONFIGURE_MAXIMUM_PORTS > 0
  DUAL_PORTED_MEMORY_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_PORTS );
#endif

#if CONFIGURE_MAXIMUM_REGIONS > 0
  REGION_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_REGIONS );
#endif

#if CONFIGURE_MAXIMUM_SEMAPHORES > 0
  SEMAPHORE_INFORMATION_DEFINE(
    CONFIGURE_MAXIMUM_SEMAPHORES,
    _CONFIGURE_SCHEDULER_COUNT
  );
#endif

#if CONFIGURE_MAXIMUM_TIMERS > 0
  TIMER_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_TIMERS );
#endif

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_OBJECTSCLASSIC_H */
