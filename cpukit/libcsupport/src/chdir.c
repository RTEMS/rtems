/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libio Internal Interface
 *
 * @brief POSIX 1003.1b - 5.2.1 - Change Current Working Directory
 */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <unistd.h>

#include <rtems/libio_.h>

int rtems_filesystem_chdir( rtems_filesystem_location_info_t *loc )
{
  int rv = 0;
  rtems_filesystem_global_location_t *global_loc =
    rtems_filesystem_location_transform_to_global( loc );
  mode_t type = rtems_filesystem_location_type( &global_loc->location );

  if ( S_ISDIR( type ) ) {
    rtems_filesystem_global_location_assign(
      &rtems_filesystem_current,
      global_loc
    );
  } else {
    rtems_filesystem_location_error( &global_loc->location, ENOTDIR );
    rtems_filesystem_global_location_release( global_loc, true );
    rv = -1;
  }

  return rv;
}

int chdir( const char *path )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_PERMS_EXEC
    | RTEMS_FS_FOLLOW_LINK;
  rtems_filesystem_location_info_t pathloc;

  rtems_filesystem_eval_path_start( &ctx, path, eval_flags );
  rtems_filesystem_eval_path_extract_currentloc( &ctx, &pathloc );
  rv = rtems_filesystem_chdir( &pathloc );
  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
