/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Change Directory
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

/**
 *  compatible with SVr4, 4.4BSD and X/OPEN - Change Directory
 */
int fchdir( int fd )
{
  int rv = 0;
  rtems_libio_t *iop;
  struct stat st;
  rtems_filesystem_location_info_t loc;

  st.st_mode = 0;
  st.st_uid = 0;
  st.st_gid = 0;

  LIBIO_GET_IOP( fd, iop );

  rtems_filesystem_instance_lock( &iop->pathinfo );
  rv = (*iop->pathinfo.handlers->fstat_h)( &iop->pathinfo, &st );
  if ( rv == 0 ) {
    bool access_ok = rtems_filesystem_check_access(
      RTEMS_FS_PERMS_EXEC,
      st.st_mode,
      st.st_uid,
      st.st_gid
    );

    if ( access_ok ) {
      rtems_filesystem_location_clone( &loc, &iop->pathinfo );
    } else {
      errno = EACCES;
      rv = -1;
    }
  }
  rtems_filesystem_instance_unlock( &iop->pathinfo );
  rtems_libio_iop_drop( iop );

  if ( rv == 0 ) {
    rv = rtems_filesystem_chdir( &loc );
  }

  return rv;
}
