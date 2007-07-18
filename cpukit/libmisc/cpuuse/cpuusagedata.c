/*
 *  CPU Usage Reporter - Shared Data
 *
 *  COPYRIGHT (c) 1989-2007
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include <rtems/cpuuse.h>

#if defined(RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS) || \
    defined(RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS)
  #include <rtems/score/timespec.h>

#endif

#ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
  struct timespec    CPU_usage_Uptime_at_last_reset;
#else
  uint32_t           CPU_usage_Ticks_at_last_reset;
#endif

