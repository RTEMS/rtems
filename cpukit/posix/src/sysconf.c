/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Get Configurable System Variables
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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

#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/seterr.h>
#include <rtems/libio_.h>

#include <sys/param.h>

/*
 *  4.8.1 Get Configurable System Variables, P1003.1b-1993, p. 95
 */

long sysconf(
  int name
)
{
  switch ( name ) {
    case _SC_CLK_TCK:
      return (long) rtems_clock_get_ticks_per_second();
    case _SC_OPEN_MAX:
      return rtems_libio_number_iops;
    case _SC_GETPW_R_SIZE_MAX:
      return 1024;
    case _SC_PAGESIZE:
      return PAGE_SIZE;
    case _SC_SYMLOOP_MAX:
      return RTEMS_FILESYSTEM_SYMLOOP_MAX;
    case _SC_NPROCESSORS_CONF:
      return (long) rtems_configuration_get_maximum_processors();
    case _SC_NPROCESSORS_ONLN:
      return (long) rtems_scheduler_get_processor_maximum();
    case _SC_POSIX_26_VERSION:
      return (long) _POSIX_26_VERSION;
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
  }
}
