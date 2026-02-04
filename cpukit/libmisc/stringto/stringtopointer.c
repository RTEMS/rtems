/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libmisc_conv_help Conversion Helpers
 *
 * @brief Convert String to Pointer (with validation)
 */

/*
 * Copyright (c) 2009 On-Line Applications Research Corporation (OAR).
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
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include <rtems/stringto.h>

/*
 *  Instantiate an error checking wrapper for strtoul/strtoull (void *)
 */

#if ( UINTPTR_MAX == ULONG_MAX )
#define STRTOFUNC( a, b, c ) \
  rtems_string_to_unsigned_long( a, (unsigned long *) b, c, 0 )
#elif ( UINTPTR_MAX == ULONG_LONG_MAX )
#define STRTOFUNC( a, b, c ) \
  rtems_string_to_unsigned_long_long( a, (unsigned long long *) b, c, 0 )
#elif ( UINTPTR_MAX == UINT_MAX )
#define STRTOFUNC( a, b, c ) \
  rtems_string_to_unsigned_int( a, (unsigned int *) b, c, 0 )
#else
/* Fallback to unsigned long */
#define STRTOFUNC( a, b, c ) \
  rtems_string_to_unsigned_long( a, (unsigned long *) b, c, 0 )
#endif

rtems_status_code rtems_string_to_pointer(
  const char *s,
  void      **n,
  char      **endptr
)
{
  return STRTOFUNC( s, n, endptr );
}
