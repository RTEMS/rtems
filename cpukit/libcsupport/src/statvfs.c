/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2009 Chris Johns <chrisj@rtems.org>
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
/*
 * The statvfs as defined by the SUS:
 *    http://www.opengroup.org/onlinepubs/009695399/basedefs/sys/statvfs.h.html
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/statvfs.h>
#include <string.h>

#include <rtems/libio_.h>

int statvfs( const char *__restrict path, struct statvfs *__restrict buf )
{
  int rv = 0;
  rtems_filesystem_eval_path_context_t ctx;
  int eval_flags = RTEMS_FS_FOLLOW_LINK;
  const rtems_filesystem_location_info_t *currentloc =
    rtems_filesystem_eval_path_start( &ctx, path, eval_flags );

  memset( buf, 0, sizeof( *buf ) );

  rv = (*currentloc->mt_entry->ops->statvfs_h)( currentloc, buf );

  rtems_filesystem_eval_path_cleanup( &ctx );

  return rv;
}
