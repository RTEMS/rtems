/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Remove a Directory
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

#include <unistd.h>

#include <rtems/libio_.h>

/**
 *   POSIX 1003.1b - 5.2.2 - Remove a Directory
 */
int rmdir( const char *path )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_REJECT_TERMINAL_DOT;
  rtems_filesystem_location_info_t parentloc;
  int parent_eval_flags = RTEMS_FS_PERMS_WRITE
    | RTEMS_FS_PERMS_EXEC
    | RTEMS_FS_FOLLOW_LINK;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start_with_parent(
      &ctx,
      path,
      eval_flags,
      &parentloc,
      parent_eval_flags
    );
  const rtems_filesystem_operations_table *ops = currentloc->mt_entry->ops;
  mode_t type = rtems_filesystem_location_type( currentloc );

  if ( S_ISDIR( type ) ) {
    if ( !rtems_filesystem_location_is_instance_root( currentloc ) ) {
      rv = (*ops->rmnod_h)( &parentloc, currentloc );
    } else {
      rtems_filesystem_eval_path_error( &ctx, EBUSY );
      rv = -1;
    }
  } else {
    rtems_filesystem_eval_path_error( &ctx, ENOTDIR );
    rv = -1;
  }

  rtems_filesystem_eval_path_cleanup_with_parent( &ctx, &parentloc );

  return rv;
}
