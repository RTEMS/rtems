/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreTimespec
 *
 * @brief This source file contains the implementation of
 *   _Timespec_To_ticks().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/timespec.h>
#include <rtems/score/todimpl.h>
#include <rtems/config.h>

/**
 *
 *  This routines converts a timespec to the corresponding number of ticks.
 */

uint32_t _Timespec_To_ticks(
  const struct timespec *time
)
{
  uint32_t  ticks;
  uint32_t  nanoseconds_per_tick;

  if ( (time->tv_sec == 0) && (time->tv_nsec == 0) )
    return 0;

  /**
   *  We should ensure the ticks not be truncated by integer division.  We
   *  need to have it be greater than or equal to the requested time.  It
   *  should not be shorter.
   */
  ticks                 = time->tv_sec * TOD_TICKS_PER_SECOND;
  nanoseconds_per_tick  = rtems_configuration_get_nanoseconds_per_tick();
  ticks                += time->tv_nsec / nanoseconds_per_tick;

  if ( (time->tv_nsec % nanoseconds_per_tick) != 0 )
    ticks += 1;

  return ticks;
}
