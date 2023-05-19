/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Rename a File
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH & Co. KG
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

#if defined(RTEMS_NEWLIB) && !defined(HAVE__RENAME_R)

#include <stdio.h>

#include <rtems/libio_.h>

/**
 *  POSIX 1003.1b - 5.3.4 - Rename a file
 */
int _rename_r(
  struct _reent *ptr RTEMS_UNUSED,
  const char    *old,
  const char    *new
)
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t old_ctx;
  int old_eval_flags = 0;
  rtems_filesystem_location_info_t old_parentloc;
  int old_parent_eval_flags = RTEMS_FS_PERMS_WRITE
    | RTEMS_FS_FOLLOW_HARD_LINK;
  const rtems_filesystem_location_info_t *old_currentloc =
    rtems_filesystem_eval_path_start_with_parent(
      &old_ctx,
      old,
      old_eval_flags,
      &old_parentloc,
      old_parent_eval_flags
    );
  rtems_filesystem_eval_path_context_t new_ctx;

  /* FIXME: This is not POSIX conform */
  int new_eval_flags = RTEMS_FS_FOLLOW_HARD_LINK
    | RTEMS_FS_MAKE
    | RTEMS_FS_EXCLUSIVE;

  const rtems_filesystem_location_info_t *new_currentloc =
    rtems_filesystem_eval_path_start( &new_ctx, new, new_eval_flags );

  rv = rtems_filesystem_location_exists_in_same_instance_as(
    old_currentloc,
    new_currentloc
  );
  if ( rv == 0 ) {
    rv = (*new_currentloc->mt_entry->ops->rename_h)(
      &old_parentloc,
      old_currentloc,
      new_currentloc,
      rtems_filesystem_eval_path_get_token( &new_ctx ),
      rtems_filesystem_eval_path_get_tokenlen( &new_ctx )
    );
  }

  rtems_filesystem_eval_path_cleanup_with_parent( &old_ctx, &old_parentloc );
  rtems_filesystem_eval_path_cleanup( &new_ctx );

  return rv;
}
#endif
