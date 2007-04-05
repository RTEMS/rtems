/**
 * @file rtems/posix/time.h
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_POSIX_TIME_H
#define _RTEMS_POSIX_TIME_H

#include <rtems/score/timespec.h>

/*
 *  _POSIX_Absolute_timeout_to_ticks
 */
int _POSIX_Absolute_timeout_to_ticks(
  const struct timespec *abstime,
  Watchdog_Interval     *ticks_out
);

#endif
