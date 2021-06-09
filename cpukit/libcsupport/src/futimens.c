/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup libcsupport
 *
 *  @brief Set file access and modification times based on file descriptor in
 *  nanoseconds.
 */

/*
 * COPYRIGHT (C) 2021 On-Line Applications Research Corporation (OAR).
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

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

/**
 *  https://pubs.opengroup.org/onlinepubs/9699919799.2008edition/functions/futimens.html
 *
 *  Set file access and modification times
 */
int futimens(
  int                    fd,
  const struct timespec  times[2]
)
{
  int rv;
  rtems_libio_t *iop;
  struct timespec new_times[2];
  const rtems_filesystem_location_info_t *currentloc = NULL;

  LIBIO_GET_IOP_WITH_ACCESS( fd, iop, LIBIO_FLAGS_READ, EBADF );

  currentloc = &iop->pathinfo;

  rv = rtems_filesystem_utime_update( times, new_times );
  if ( rv != 0 ) {
    rtems_libio_iop_drop( iop );
    return rv;
  }

  rv = rtems_filesystem_utime_check_permissions( currentloc, times );
  if ( rv != 0 ) {
    rtems_libio_iop_drop( iop );
    return rv;
  }

  rv = (*currentloc->mt_entry->ops->utimens_h)(
    currentloc,
    new_times
  );

  rtems_libio_iop_drop( iop );

  return rv;
}
