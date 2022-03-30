/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Get Directory Entries
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2013.
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

#include <errno.h>

#include <rtems/score/objectimpl.h>
#include <rtems/libio_.h>
#include <rtems/seterr.h>

/*
 *  Prototypes to avoid warnings
 */
int getdents(
  int   dd_fd,
  char *dd_buf,
  int   dd_len
);

/**
 *  SVR4 and SVID extension required by Newlib readdir() family.
 *
 *  This routine will dd_len / (sizeof dirent) directory entries relative to
 *  the current directory position index. These entries will be placed in
 *  character array pointed to by -dd_buf-
 */
int getdents(
  int   dd_fd,
  char *dd_buf,
  int   dd_len
)
{
  rtems_libio_t *iop;
  mode_t type;

  /*
   *  Get the file control block structure associated with the file descriptor
   */
  iop = rtems_libio_iop( dd_fd );

  /*
   *  Make sure we are working on a directory
   */
  type = rtems_filesystem_location_type( &iop->pathinfo );
  if ( !S_ISDIR( type ) )
    rtems_set_errno_and_return_minus_one( ENOTDIR );

  /*
   *  Return the number of bytes that were actually transfered as a result
   *  of the read attempt.
   */
  return (*iop->pathinfo.handlers->read_h)( iop, dd_buf, dd_len  );
}
