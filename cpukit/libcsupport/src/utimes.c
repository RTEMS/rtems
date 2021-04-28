/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup libcsupport
 *
 *  @brief Set file access and modification times in milliseconds.
 */

/*
 * COPYRIGHT (C) 1989, 2021 On-Line Applications Research Corporation (OAR).
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

#include <sys/time.h>
#include <rtems/score/todimpl.h>

#include <fcntl.h>

/**
 *  https://pubs.opengroup.org/onlinepubs/9699919799.2008edition/functions/futimens.html
 *
 *  Set file access and modification times
 */
int utimes(
  const char           *path,
  const struct timeval  times[2]
)
{
  struct timespec new_times[2];

  if ( times == NULL ) {
    return utimensat( AT_FDCWD, path, NULL , 0 );
  }

  _Timespec_Set(
    &new_times[0],
    times[0].tv_sec,
    times[0].tv_usec * TOD_NANOSECONDS_PER_MICROSECOND
  );
  _Timespec_Set(
    &new_times[1],
    times[1].tv_sec,
    times[1].tv_usec * TOD_NANOSECONDS_PER_MICROSECOND
  );

  return utimensat( AT_FDCWD, path, new_times, 0 );
}
