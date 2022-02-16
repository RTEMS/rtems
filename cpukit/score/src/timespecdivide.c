/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreTimespec
 *
 * @brief This source file contains the implementation of
 *   _Timespec_Divide().
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

void _Timespec_Divide(
  const struct timespec *lhs,
  const struct timespec *rhs,
  uint32_t              *ival_percentage,
  uint32_t              *fval_percentage
)
{
  uint64_t left, right, answer;

  /*
   *  For math simplicity just convert the timespec to nanoseconds
   *  in a 64-bit integer.
   */
  left   = lhs->tv_sec * (uint64_t)TOD_NANOSECONDS_PER_SECOND;
  left  += lhs->tv_nsec;
  right  = rhs->tv_sec * (uint64_t)TOD_NANOSECONDS_PER_SECOND;
  right += rhs->tv_nsec;

  if ( right == 0 ) {
    *ival_percentage = 0;
    *fval_percentage = 0;
    return;
  }

  /*
   *  Put it back in the timespec result.
   *
   *  TODO: Rounding on the last digit of the fval.
   */

  answer = (left * 100000) / right;

  *ival_percentage = answer / 1000;
  *fval_percentage = answer % 1000;
}
