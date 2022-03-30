/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Get File Status
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2013.
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

/*
 *  lstat() and stat() share the same implementation with a minor
 *  difference on how links are evaluated.
 */

#ifndef _STAT_NAME
#define _STAT_NAME         stat
#define _STAT_R_NAME       _stat_r
#define _STAT_FOLLOW_LINKS RTEMS_FS_FOLLOW_LINK
#endif

#include <sys/stat.h>
#include <string.h>

#include <rtems/libio_.h>

/*
 *  Prototype to avoid warnings
 */
int _STAT_NAME( const char *path, struct stat *buf );

/**
 *  POSIX 1003.1b 5.6.2 - Get File Status
 * 
 *  Reused from lstat().
 */
int _STAT_NAME( const char *path, struct stat *buf )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = _STAT_FOLLOW_LINKS;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  memset( buf, 0, sizeof( *buf ) );

  rv = (*currentloc->handlers->fstat_h)( currentloc, buf );

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}

#if defined(RTEMS_NEWLIB)

#include <reent.h>

/*
 *  Prototype to avoid warnings
 */
int _STAT_R_NAME(
  struct _reent *ptr RTEMS_UNUSED,
  const char    *path,
  struct stat   *buf
);

/**
 *  This is the Newlib dependent reentrant version of stat() and lstat().
 */
int _STAT_R_NAME(
  struct _reent *ptr RTEMS_UNUSED,
  const char    *path,
  struct stat   *buf
)
{
  return _STAT_NAME( path, buf );
}
#endif
