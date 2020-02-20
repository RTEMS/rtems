/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate Unlimited Objects Configuration Options
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

#ifndef _RTEMS_CONFDEFS_UNLIMITED_H
#define _RTEMS_CONFDEFS_UNLIMITED_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#ifdef CONFIGURE_UNLIMITED_OBJECTS

#include <rtems/config.h>

#if !defined(CONFIGURE_UNIFIED_WORK_AREAS) \
  && !defined(CONFIGURE_EXECUTIVE_RAM_SIZE) \
  && !defined(CONFIGURE_MEMORY_OVERHEAD)
  #error "CONFIGURE_UNLIMITED_OBJECTS requires one of CONFIGURE_UNIFIED_WORK_AREAS, CONFIGURE_EXECUTIVE_RAM_SIZE, and CONFIGURE_MEMORY_OVERHEAD"
#endif

#ifndef CONFIGURE_UNLIMITED_ALLOCATION_SIZE
  #define CONFIGURE_UNLIMITED_ALLOCATION_SIZE 8
#endif

#ifndef CONFIGURE_MAXIMUM_TASKS
  #define CONFIGURE_MAXIMUM_TASKS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_TIMERS
  #define CONFIGURE_MAXIMUM_TIMERS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_SEMAPHORES
  #define CONFIGURE_MAXIMUM_SEMAPHORES \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_MESSAGE_QUEUES
  #define CONFIGURE_MAXIMUM_MESSAGE_QUEUES \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_PARTITIONS
  #define CONFIGURE_MAXIMUM_PARTITIONS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_REGIONS
  #define CONFIGURE_MAXIMUM_REGIONS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_PORTS
  #define CONFIGURE_MAXIMUM_PORTS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_PERIODS
  #define CONFIGURE_MAXIMUM_PERIODS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_BARRIERS
  #define CONFIGURE_MAXIMUM_BARRIERS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_KEYS
  #define CONFIGURE_MAXIMUM_POSIX_KEYS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS
  #define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
  #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_SEMAPHORES
  #define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_SHMS
  #define CONFIGURE_MAXIMUM_POSIX_SHMS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#ifndef CONFIGURE_MAXIMUM_POSIX_THREADS
  #define CONFIGURE_MAXIMUM_POSIX_THREADS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#if defined(RTEMS_POSIX_API) && !defined(CONFIGURE_MAXIMUM_POSIX_TIMERS)
  #define CONFIGURE_MAXIMUM_POSIX_TIMERS \
    rtems_resource_unlimited( CONFIGURE_UNLIMITED_ALLOCATION_SIZE )
#endif

#endif /* CONFIGURE_UNLIMITED_OBJECTS */

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_UNLIMITED_H */
