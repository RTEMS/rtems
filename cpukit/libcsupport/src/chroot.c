/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief Change Root Directory
 */

/*
 * Copyright (c) 2001 Fernando Ruiz Casas <fruizcasas@gmail.com>
 *
 * COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * Modifications to support reference counting in the file system are
 * Copyright (c) 2012 embedded brains GmbH & Co. KG
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

#include <string.h>
#include <unistd.h>

#include <rtems/libio_.h>

int chroot( const char *path )
{
  int rv = 0;
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_PERMS_EXEC
    | RTEMS_FS_FOLLOW_LINK;
  rtems_filesystem_location_info_t loc;
  rtems_filesystem_global_location_t *new_current_loc;

  /*
   * We use the global environment for path evaluation.  This makes it possible
   * to escape from a chroot environment referencing an unmounted file system.
   */
  rtems_filesystem_eval_path_start_with_root_and_current(
    &ctx,
    path,
    strlen( path ),
    eval_flags,
    &rtems_global_user_env.root_directory,
    &rtems_global_user_env.current_directory
  );

  rtems_filesystem_eval_path_extract_currentloc( &ctx, &loc );
  new_current_loc = rtems_filesystem_location_transform_to_global( &loc );
  if ( !rtems_filesystem_global_location_is_null( new_current_loc ) ) {
    rtems_filesystem_global_location_t *new_root_loc =
      rtems_filesystem_global_location_obtain( &new_current_loc );
    mode_t type = rtems_filesystem_location_type( &new_root_loc->location );

    if ( S_ISDIR( type ) ) {
      sc = rtems_libio_set_private_env();
      if (sc == RTEMS_SUCCESSFUL) {
        rtems_filesystem_global_location_assign(
          &rtems_filesystem_root,
          new_root_loc
        );
        rtems_filesystem_global_location_assign(
          &rtems_filesystem_current,
          new_current_loc
        );
      } else {
        if (sc != RTEMS_UNSATISFIED) {
          errno = ENOMEM;
        }
        rv = -1;
      }
    } else {
      rtems_filesystem_location_error( &new_root_loc->location, ENOTDIR );
      rv = -1;
    }

    if ( rv != 0 ) {
      rtems_filesystem_global_location_release( new_root_loc, true );
    }
  } else {
    rv = -1;
  }

  rtems_filesystem_eval_path_cleanup( &ctx );

  if ( rv != 0 ) {
    rtems_filesystem_global_location_release( new_current_loc, false );
  }

  return rv;
}
