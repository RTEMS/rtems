/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup libcsupport
 *
 *  @brief Set file access and modification times in nanoseconds.
 */

/*
 * COPYRIGHT (C) 2021 On-Line Applications Research Corporation (OAR).
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

#include <rtems/libio_.h>
#include <rtems/score/todimpl.h>

#include <fcntl.h>
#include <string.h>

/*
 * Make sure that tv_nsec is either UTIME_NOW, UTIME_OMIT, a value
 * greater than zero, or a value less-than a billion.
 *
 * These guidelines come from the description of the EINVAL errors on
 * https://pubs.opengroup.org/onlinepubs/9699919799/functions/futimens.html
 */
bool rtems_filesystem_utime_tv_nsec_valid(struct timespec time)
{
  if ( time.tv_nsec == UTIME_NOW ) {
    return true;
  }

  if ( time.tv_nsec == UTIME_OMIT ) {
    return true;
  }

  if ( time.tv_nsec < 0 ) {
    return false;
  }

  if ( time.tv_nsec >= TOD_NANOSECONDS_PER_SECOND ) {
    return false;
  }

  return true;
}

/* Determine whether the access and modified timestamps can be updated */
int rtems_filesystem_utime_check_permissions(
  const rtems_filesystem_location_info_t * currentloc,
  const struct timespec times[2]
)
{
  struct stat st = {};
  int rv;
  bool write_access;

  rv = (*currentloc->handlers->fstat_h)( currentloc, &st );
  if ( rv != 0 ) {
    rtems_set_errno_and_return_minus_one( ENOENT );
  }

  write_access = rtems_filesystem_check_access(
    RTEMS_FS_PERMS_WRITE,
    st.st_mode,
    st.st_uid,
    st.st_gid
  );

  /*
   * The logic for the EACCES error is an inverted subset of the EPERM
   * conditional according to the POSIX standard.
   */
  if ( (times == NULL) ||
     ( (times[0].tv_nsec == UTIME_NOW) && (times[1].tv_nsec == UTIME_NOW) )) {
      if ( !write_access ) {
        rtems_set_errno_and_return_minus_one( EACCES );
      }
  } else {
    if ( times[0].tv_nsec != UTIME_OMIT || times[1].tv_nsec != UTIME_OMIT ) {
      if ( !write_access ) {
        rtems_set_errno_and_return_minus_one( EPERM );
      }
    }
  }

  return 0;
}

/*
 * Determine if the current time needs to be gotten, and then check
 * whether the values in times are valid.
 */
int rtems_filesystem_utime_update(
  const struct timespec times[2],
  struct timespec new_times[2]
)
{
  bool got_time = false;
  struct timespec now;

  /*
   * If times is NULL, it's equivalent to adding UTIME_NOW in both time
   * elements
   */
  if ( times == NULL ) {
    _Timespec_Set( &new_times[0], 0, UTIME_NOW );
    _Timespec_Set( &new_times[1], 0, UTIME_NOW );
  } else {
    new_times[0] = times[0];
    new_times[1] = times[1];
  }

  if ( new_times[0].tv_nsec == UTIME_NOW ) {
    clock_gettime( CLOCK_REALTIME, &now );
    new_times[0] = now;
    got_time = true;
  }

  if ( new_times[1].tv_nsec == UTIME_NOW ) {
    if ( !got_time ) {
      clock_gettime( CLOCK_REALTIME, &now );
    }
    new_times[1] = now;
  }

  if ( !_Timespec_Is_non_negative( &new_times[0] ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( !_Timespec_Is_non_negative( &new_times[1] ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( !rtems_filesystem_utime_tv_nsec_valid( new_times[0] ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( !rtems_filesystem_utime_tv_nsec_valid( new_times[1] ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  return 0;
}

/**
 *  https://pubs.opengroup.org/onlinepubs/9699919799.2008edition/functions/futimens.html
 *
 *  Set file access and modification times
 */
int utimensat(
  int                    fd,
  const char            *path,
  const struct timespec  times[2],
  int                    flag
)
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_LINK;
  const rtems_filesystem_location_info_t *currentloc = NULL;
  struct timespec new_times[2];

  /*
   * RTEMS does not currently support operating on a real file descriptor
   */
  if ( fd != AT_FDCWD ) {
    rtems_set_errno_and_return_minus_one( ENOSYS );
  }

  /*
   * RTEMS does not currently support AT_SYMLINK_NOFOLLOW
   */
  if ( flag != 0 ) {
    rtems_set_errno_and_return_minus_one( ENOSYS );
  }

  rv = rtems_filesystem_utime_update( times, new_times );
  if ( rv != 0 ) {
    return rv;
  }

  currentloc = rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  rv = rtems_filesystem_utime_check_permissions( currentloc, times );
  if ( rv != 0 ) {
    rtems_filesystem_eval_path_cleanup( &ctx );
    return rv;
  }

  rv = (*currentloc->mt_entry->ops->utimens_h)(
    currentloc,
    new_times
  );

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
