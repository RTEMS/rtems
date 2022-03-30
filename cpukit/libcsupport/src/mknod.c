/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Create a Special or Ordinary File 
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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

#include <sys/stat.h>

#include <rtems/libio_.h>

int rtems_filesystem_mknod(
  const rtems_filesystem_location_info_t *parentloc,
  const char *name,
  size_t namelen,
  mode_t mode,
  dev_t dev
)
{
  int rv = 0;

  mode &= ~rtems_filesystem_umask;

  switch (mode & S_IFMT) {
    case S_IFBLK:
    case S_IFCHR:
    case S_IFDIR:
    case S_IFIFO:
    case S_IFREG:
      break;
    default:
      errno = EINVAL;
      rv = -1;
      break;
  }
  
  if ( rv == 0 ) {
    const rtems_filesystem_operations_table *ops = parentloc->mt_entry->ops;

    rv = (*ops->mknod_h)( parentloc, name, namelen, mode, dev );
  }

  return rv;
}

/**
 * This routine is not defined in the POSIX 1003.1b standard but is
 *  commonly supported on most UNIX and POSIX systems.  It is the
 *  foundation for creating file system objects.
 */
int mknod( const char *path, mode_t mode, dev_t dev )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_LINK
    | RTEMS_FS_MAKE
    | RTEMS_FS_EXCLUSIVE
    | (S_ISDIR(mode) ? RTEMS_FS_ACCEPT_RESIDUAL_DELIMITERS : 0);
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  rv = rtems_filesystem_mknod(
    currentloc,
    rtems_filesystem_eval_path_get_token( &ctx ),
    rtems_filesystem_eval_path_get_tokenlen( &ctx ),
    mode,
    dev
  );

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
