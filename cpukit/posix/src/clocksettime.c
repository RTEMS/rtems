/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Set Time of Clock
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

#include <time.h>

#include <rtems/score/todimpl.h>
#include <rtems/seterr.h>

/*
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_settime(
  clockid_t              clock_id,
  const struct timespec *tp
)
{
  Status_Control status;

  if ( !tp )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( clock_id == CLOCK_REALTIME ) {
    ISR_lock_Context lock_context;

    status = _TOD_Is_valid_new_time_of_day( tp );
    if ( status != STATUS_SUCCESSFUL ) {
      rtems_set_errno_and_return_minus_one( STATUS_GET_POSIX( status ) );
    }

    _TOD_Lock();
    _TOD_Acquire( &lock_context );
      status = _TOD_Set( tp, &lock_context );
    _TOD_Unlock();

    if ( status != STATUS_SUCCESSFUL ) {
      rtems_set_errno_and_return_minus_one( STATUS_GET_POSIX( status ) );
    }
  }
#ifdef _POSIX_CPUTIME
  else if ( clock_id == CLOCK_PROCESS_CPUTIME_ID ) {
    rtems_set_errno_and_return_minus_one( ENOSYS );
  }
#endif
#ifdef _POSIX_THREAD_CPUTIME
  else if ( clock_id == CLOCK_THREAD_CPUTIME_ID ) {
    rtems_set_errno_and_return_minus_one( ENOSYS );
  }
#endif
  else {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  return 0;
}
