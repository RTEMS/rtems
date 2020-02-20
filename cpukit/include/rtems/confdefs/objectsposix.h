/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate POSIX API Objects Configuration Options
 *
 * For the POSIX thread objects configuration see <rtems/confdefs/threads.h>.
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

#ifndef _RTEMS_CONFDEFS_OBJECTSPOSIX_H
#define _RTEMS_CONFDEFS_OBJECTSPOSIX_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#include <rtems/confdefs/threads.h>
#include <rtems/confdefs/wkspacesupport.h>
#include <rtems/config.h>

#include <limits.h>

#define _Configure_Memory_for_named_objects( _number, _size ) \
  ( rtems_resource_maximum_per_allocation( _number ) \
    * _Configure_From_workspace( _POSIX_PATH_MAX + 1 ) )

#ifndef CONFIGURE_MAXIMUM_POSIX_KEYS
  #define CONFIGURE_MAXIMUM_POSIX_KEYS 0
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS
  #define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS \
    ( CONFIGURE_MAXIMUM_POSIX_KEYS * \
     ( CONFIGURE_MAXIMUM_POSIX_THREADS + CONFIGURE_MAXIMUM_TASKS ) )
#endif

#if CONFIGURE_MAXIMUM_POSIX_KEYS > 0 \
  && CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS < CONFIGURE_MAXIMUM_POSIX_KEYS
  #error "CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS must be greater than or equal to CONFIGURE_MAXIMUM_POSIX_KEYS"
#endif

#if CONFIGURE_MAXIMUM_POSIX_KEYS == 0 \
  && CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS > 0
  #warning "If CONFIGURE_MAXIMUM_POSIX_KEYS is zero, then CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS should be zero as well"
#endif

#if CONFIGURE_MAXIMUM_POSIX_KEYS > 0 || CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS > 0
  #include <rtems/posix/key.h>
#endif

#if CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES > 0
  #include <rtems/posix/mqueue.h>
#endif

#if CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS > 0
  #include <rtems/posix/psignal.h>
#endif

#if CONFIGURE_MAXIMUM_POSIX_SEMAPHORES > 0
  #include <rtems/posix/semaphore.h>
#endif

#if CONFIGURE_MAXIMUM_POSIX_SHMS > 0
  #include <rtems/posix/shm.h>
#endif

#if CONFIGURE_MAXIMUM_POSIX_TIMERS > 0
  #include <rtems/posix/timer.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIGURE_MAXIMUM_POSIX_KEYS > 0
  POSIX_KEYS_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_POSIX_KEYS );
#endif

#if CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS > 0
  POSIX_Keys_Key_value_pair _POSIX_Keys_Key_value_pairs[
    rtems_resource_maximum_per_allocation(
      CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS
    )
  ];

  const uint32_t _POSIX_Keys_Key_value_pair_maximum =
    CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS;
#endif

#if CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES > 0
  POSIX_MESSAGE_QUEUE_INFORMATION_DEFINE(
    CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
  );

  #define _CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES \
    _Configure_Memory_for_named_objects( \
       CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES, \
       sizeof( POSIX_Message_queue_Control ) \
     )
#else
  #define _CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES 0
#endif

#if CONFIGURE_MAXIMUM_POSIX_SEMAPHORES > 0
  POSIX_SEMAPHORE_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_POSIX_SEMAPHORES );

  #define _CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES \
    _Configure_Memory_for_named_objects( \
       CONFIGURE_MAXIMUM_POSIX_SEMAPHORES, \
       sizeof( POSIX_Semaphore_Control ) \
     )
#else
  #define _CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES 0
#endif

#if CONFIGURE_MAXIMUM_POSIX_SHMS > 0
  POSIX_SHM_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_POSIX_SHMS );

  #define _CONFIGURE_MEMORY_FOR_POSIX_SHMS \
    _Configure_Memory_for_named_objects( \
       CONFIGURE_MAXIMUM_POSIX_SHMS, \
       sizeof( POSIX_Shm_Control ) \
     )
#else
  #define _CONFIGURE_MEMORY_FOR_POSIX_SHMS 0
#endif

#if CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS > 0
  #ifndef RTEMS_POSIX_API
    #error "CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS must be zero if POSIX API is disabled"
  #endif

  const uint32_t _POSIX_signals_Maximum_queued_signals =
    CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS;

  POSIX_signals_Siginfo_node _POSIX_signals_Siginfo_nodes[
    CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS
  ];

  #define _CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS \
    _Configure_From_workspace( CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS \
      * sizeof( POSIX_signals_Siginfo_node ) )
#else
  #define _CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS 0
#endif

#if CONFIGURE_MAXIMUM_POSIX_TIMERS > 0
  #ifndef RTEMS_POSIX_API
    #error "CONFIGURE_MAXIMUM_POSIX_TIMERS must be zero if POSIX API is disabled"
  #endif

  POSIX_TIMER_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_POSIX_TIMERS );
#endif

#ifdef __cplusplus
}
#endif

#define _CONFIGURE_MEMORY_FOR_POSIX_OBJECTS \
  ( _CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES \
    + _CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS \
    + _CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES \
    + _CONFIGURE_MEMORY_FOR_POSIX_SHMS )

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_OBJECTSPOSIX_H */
