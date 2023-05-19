/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup LibIOFSOps File System Operations 
 *
 * @brief RTEMS Default File System reposits the offset of the open file fd
 */

/*
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

#include <rtems/libio_.h>

off_t rtems_filesystem_default_lseek_file(
  rtems_libio_t *iop,
  off_t offset,
  int whence
)
{
  off_t rv = 0;
  off_t reference_offset;
  struct stat st;

  switch ( whence ) {
    case SEEK_SET:
      reference_offset = 0;
      break;
    case SEEK_CUR:
      reference_offset = iop->offset;
      break;
    case SEEK_END:
      st.st_size = 0;
      rv = (*iop->pathinfo.handlers->fstat_h)( &iop->pathinfo, &st );
      reference_offset = st.st_size;
      break;
    default:
      errno = EINVAL;
      rv = -1;
      break;
  }

  if ( rv == 0 ) {
    off_t new_offset = reference_offset + offset;

    if (
      (offset >= 0 && new_offset >= reference_offset)
        || (offset < 0 && new_offset < reference_offset)
    ) {
      if ( new_offset >= 0 ) {
        iop->offset = new_offset;
        rv = new_offset;
      } else {
        errno = EINVAL;
        rv = -1;
      }
    } else {
      errno = EOVERFLOW;
      rv = -1;
    }
  }

  return rv;
}
