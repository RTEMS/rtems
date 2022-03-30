/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Configurable Pathname Varables
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

#include <rtems/libio_.h>
#include <rtems/seterr.h>

#include <unistd.h>
#include <errno.h>

/**
 *  POSIX 1003.1b - 5.7.1 - Configurable Pathname Varables
 */
long fpathconf(
  int   fd,
  int   name
)
{
  long                                    return_value;
  rtems_libio_t                          *iop;
  const rtems_filesystem_limits_and_options_t *the_limits;

  LIBIO_GET_IOP( fd, iop );

  /*
   *  Now process the information request.
   */

  the_limits = iop->pathinfo.mt_entry->pathconf_limits_and_options;

  switch ( name ) {
    case _PC_LINK_MAX:
      return_value = the_limits->link_max;
      break;
    case _PC_MAX_CANON:
      return_value = the_limits->max_canon;
      break;
    case _PC_MAX_INPUT:
      return_value = the_limits->max_input;
      break;
    case _PC_NAME_MAX:
      return_value = the_limits->name_max;
      break;
    case _PC_PATH_MAX:
      return_value = the_limits->path_max;
      break;
    case _PC_PIPE_BUF:
      return_value = the_limits->pipe_buf;
      break;
    case _PC_CHOWN_RESTRICTED:
      return_value = the_limits->posix_chown_restrictions;
      break;
    case _PC_NO_TRUNC:
      return_value = the_limits->posix_no_trunc;
      break;
    case _PC_VDISABLE:
      return_value = the_limits->posix_vdisable;
      break;
    case _PC_ASYNC_IO:
      return_value = the_limits->posix_async_io;
      break;
    case _PC_PRIO_IO:
      return_value = the_limits->posix_prio_io;
      break;
    case _PC_SYNC_IO:
      return_value = the_limits->posix_sync_io;
      break;
    default:
      errno = EINVAL;
      return_value = -1;
      break;
  }

  rtems_libio_iop_drop( iop );
  return return_value;
}
