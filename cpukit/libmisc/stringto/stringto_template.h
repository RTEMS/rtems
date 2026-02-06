/* SPDX-License-Identifier: BSD-2-Clause */

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

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#include <math.h>
#include <rtems/rtems/status.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*
 * This file is designed to be included multiple times to instantiate
 * it and should NOT be protected against multiple inclusions.
 */

#if !defined( STRING_TO_FLOAT ) && !defined( STRING_TO_INTEGER )
#error "Neither STRING_TO_FLOAT nor STRING_TO_INTEGER defined"
#endif

#if defined( STRING_TO_FLOAT ) && defined( STRING_TO_INTEGER )
#error "Both STRING_TO_FLOAT nor STRING_TO_INTEGER defined"
#endif

#ifndef STRING_TO_TYPE
#error "STRING_TO_TYPE not defined"
#endif

#ifndef STRING_TO_NAME
#error "STRING_TO_NAME not defined"
#endif

#ifndef STRING_TO_METHOD
#error "STRING_TO_METHOD not defined"
#endif

#ifndef STRING_TO_MAX
#error "STRING_TO_MAX not defined"
#endif

#undef ZERO
#ifdef STRING_TO_FLOAT
#define ZERO 0.0
#elif defined( STRING_TO_INTEGER )
#define ZERO 0
#endif

rtems_status_code STRING_TO_NAME(
  const char     *s,
  STRING_TO_TYPE *n,
  char          **endptr
#if defined( STRING_TO_INTEGER )
  ,
  int base
#endif
)
{
#ifdef STRING_TO_RESULT_TYPE
  STRING_TO_RESULT_TYPE result;
#else
  STRING_TO_TYPE result;
#endif
  char *end;

  if ( NULL == s ) {
    return RTEMS_INVALID_ADDRESS;
  }

  if ( NULL == n ) {
    return RTEMS_INVALID_ADDRESS;
  }

  errno = 0;
  *n = 0;

#ifdef STRING_TO_FLOAT
  result = STRING_TO_METHOD( s, &end );
#elif defined( STRING_TO_INTEGER )
  result = STRING_TO_METHOD( s, &end, base );
#endif

  /* If the user wants the end pointer back, then return it. */
  if ( endptr ) {
    *endptr = end;
  }

  /* nothing was converted */
  if ( end == s ) {
    return RTEMS_NOT_DEFINED;
  }

  /* there was a conversion error */
  if ( ( result == ZERO ) && errno ) {
    return RTEMS_INVALID_NUMBER;
  }

#ifdef STRING_TO_MAX
  /* there was an overflow */
  if ( ( result == STRING_TO_MAX ) && ( errno == ERANGE ) ) {
    return RTEMS_INVALID_NUMBER;
  }
#endif

#ifdef STRING_TO_MIN
  /* there was an underflow */
  if ( ( result == STRING_TO_MIN ) && ( errno == ERANGE ) ) {
    return RTEMS_INVALID_NUMBER;
  }
#endif

#ifdef STRING_TO_UCHAR_MAX
  /* special case for uchar */
  if ( result > STRING_TO_UCHAR_MAX ) {
    errno = ERANGE;
    return RTEMS_INVALID_NUMBER;
  }
#endif

  *n = result;
  return RTEMS_SUCCESSFUL;
}
