/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Get the Date and Time
 *  @ingroup libcsupport
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

#if defined(RTEMS_NEWLIB)
/*
 *  Needed to get the prototype for the newlib helper method
 */
#define _LIBC

#include <sys/time.h>
#include <reent.h>
#include <errno.h>
#include <rtems/score/todimpl.h>
#include <rtems/seterr.h>

#if defined(RTEMS_NEWLIB) && !defined(HAVE_GETTIMEOFDAY)

/** 
 *  SVR4 and BSD4.3 extension required by Newlib
 * 
 *  @note The solaris gettimeofday does not have a second parameter.
 */
int gettimeofday(
  struct timeval *__restrict tp,
  void *__restrict __tz RTEMS_UNUSED
)
{
  /* struct timezone* tzp = (struct timezone*) __tz; */
  if ( !tp )
    rtems_set_errno_and_return_minus_one( EFAULT );

  /*
   *  POSIX does not seem to allow for not having a TOD so we just
   *  grab the time of day.
   */
  _TOD_Get_timeval( tp );

  /*
   *  Timezone information ignored by the OS proper.   Per email
   *  with Eric Norum, this is how GNU/Linux, Solaris, and MacOS X
   *  do it.  This puts us in good company.
   */

  return 0;
}
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE__GETTIMEOFDAY_R)

#include <sys/reent.h>

/**
 *  "Reentrant" version
 */
int _gettimeofday_r(
  struct _reent   *ignored_reentrancy_stuff RTEMS_UNUSED,
  struct timeval  *tp,
  void           *__tz
)
{
  struct timezone *tzp = __tz;
  return gettimeofday( tp, tzp );
}
#endif

#endif /* defined(RTEMS_NEWLIB) */
