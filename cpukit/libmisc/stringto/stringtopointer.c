/*
 *  COPYRIGHT (c) 2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2011  Ralf Corsépius, Ulm, Germany.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

#include <rtems/stringto.h>

/*
 *  Instantiate an error checking wrapper for strtoul/strtoull (void *)
 */

#if (UINTPTR_MAX == ULONG_MAX)
#define STRTOFUNC(a,b,c)	rtems_string_to_unsigned_long(a, (unsigned long*) b, c, 0)
#elif (UINTPTR_MAX == ULONG_LONG_MAX)
#define STRTOFUNC(a,b,c)	rtems_string_to_unsigned_long_long(a, (unsigned long long*) b, c, 0)
#elif (UINTPTR_MAX == UINT_MAX)
#define STRTOFUNC(a,b,c)	rtems_string_to_unsigned_int(a, (unsigned int*) b, c, 0)
#else
/* Fallback to unsigned long */
#define STRTOFUNC(a,b,c)	rtems_string_to_unsigned_long(a, (unsigned long*) b, c, 0)
#endif

rtems_status_code rtems_string_to_pointer (
  const char *s,
  void **n,
  char **endptr
)
{
  return STRTOFUNC( s, n, endptr );
}
