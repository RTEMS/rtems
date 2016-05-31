/**
 * @file
 *
 * @brief CPU Usage Data
 * @ingroup libmisc_cpuuse CPU Usage
 *
 * CPU Usage Reporter - Shared Data
 */

/*
 *  COPYRIGHT (c) 1989-2009
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cpuuseimpl.h"

Timestamp_Control  CPU_usage_Uptime_at_last_reset;
