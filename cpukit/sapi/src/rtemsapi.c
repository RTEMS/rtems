/**
 * @file
 *
 * @brief Initializes the RTEMS API
 *
 * @ingroup ClassicRTEMS
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#define RTEMS_API_INIT

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/rtemsapi.h>

#include <rtems/rtems/intr.h>
#include <rtems/rtems/barrierimpl.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/dpmemimpl.h>
#include <rtems/rtems/ratemonimpl.h>

void _RTEMS_API_Initialize(void)
{
  _Dual_ported_memory_Manager_initialization();
  _Rate_monotonic_Manager_initialization();
  _Barrier_Manager_initialization();
}
