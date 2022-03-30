/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Change File Modes
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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
#include <string.h>

#include <rtems/libio_.h>

int rtems_filesystem_chmod(
  const rtems_filesystem_location_info_t *loc,
  mode_t mode
)
{
  const rtems_filesystem_mount_table_entry_t *mt_entry = loc->mt_entry;
  int rv;

  if ( mt_entry->writeable || rtems_filesystem_location_is_null( loc ) ) {
    struct stat st;

    memset( &st, 0, sizeof(st) );

    rv = (*loc->handlers->fstat_h)( loc, &st );
    if ( rv == 0 ) {
      uid_t uid = geteuid();

      if ( uid == 0 || st.st_uid == uid ) {
        mode_t mask = S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX;

        mode = (st.st_mode & ~mask) | (mode & mask);

        rv = (*mt_entry->ops->fchmod_h)( loc, mode );
      } else {
        errno = EPERM;
        rv = -1;
      }
    }
  } else {
    errno = EROFS;
    rv = -1;
  }

  return rv;
}

/**
 *  POSIX 1003.1b 5.6.4 - Change File Modes
 */
int fchmod( int fd, mode_t mode )
{
  int rv;
  rtems_libio_t *iop;

  LIBIO_GET_IOP( fd, iop );

  rtems_filesystem_instance_lock( &iop->pathinfo );

  rv = rtems_filesystem_chmod( &iop->pathinfo, mode );

  rtems_filesystem_instance_unlock( &iop->pathinfo );

  rtems_libio_iop_drop( iop );

  return rv;
}
