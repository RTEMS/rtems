/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Create a new link
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
 *  link() - POSIX 1003.1b - 5.3.4 - Create a new link
 */
int link( const char *path1, const char *path2 )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx_1;
  rtems_filesystem_eval_path_context_t ctx_2;
  int eval_flags_1 = RTEMS_FS_FOLLOW_LINK;
  int eval_flags_2 = RTEMS_FS_FOLLOW_LINK
    | RTEMS_FS_MAKE
    | RTEMS_FS_EXCLUSIVE;
  const rtems_filesystem_location_info_t *currentloc_1 =
    rtems_filesystem_eval_path_start( &ctx_1, path1, eval_flags_1 );
  const rtems_filesystem_location_info_t *currentloc_2 =
    rtems_filesystem_eval_path_start( &ctx_2, path2, eval_flags_2 );

  rv = rtems_filesystem_location_exists_in_same_instance_as(
    currentloc_1,
    currentloc_2
  );
  if ( rv == 0 ) {
    rv = (*currentloc_2->mt_entry->ops->link_h)(
      currentloc_2,
      currentloc_1,
      rtems_filesystem_eval_path_get_token( &ctx_2 ),
      rtems_filesystem_eval_path_get_tokenlen( &ctx_2 )
    );
  }

  rtems_filesystem_eval_path_cleanup( &ctx_1 );
  rtems_filesystem_eval_path_cleanup( &ctx_2 );

  return rv;
}

#if defined(RTEMS_NEWLIB)

#include <reent.h>

/**
 *  This is the Newlib dependent reentrant version of link().
 */
int _link_r(
  struct _reent *ptr RTEMS_UNUSED,
  const char    *path1,
  const char    *path2
)
{
  return link( path1, path2 );
}
#endif
