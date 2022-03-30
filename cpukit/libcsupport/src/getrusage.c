/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <sys/resource.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/seterr.h>

int getrusage(int who, struct rusage *usage)
{
  struct timespec uptime;
  struct timeval  rtime;

  if ( !usage )
    rtems_set_errno_and_return_minus_one( EFAULT );

  /*
   *  RTEMS only has a single process so there are no children.
   *  The single process has been running since the system
   *  was booted and since there is no distinction between system
   *  and user time, we will just report the uptime.
   */
  if (who == RUSAGE_SELF) {
    rtems_clock_get_uptime( &uptime );

    rtime.tv_sec  = uptime.tv_sec;
    rtime.tv_usec = uptime.tv_nsec / 1000;

    usage->ru_utime = rtime;
    usage->ru_stime = rtime;

    return 0;
  }

  if (who == RUSAGE_CHILDREN) {
    rtems_set_errno_and_return_minus_one( ENOSYS );
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}

