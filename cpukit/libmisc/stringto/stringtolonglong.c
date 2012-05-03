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

#include <rtems/stringto.h>

/* c99 has LLONG_MAX instead of LONG_LONG_MAX */
#ifndef LONG_LONG_MAX
#define LONG_LONG_MAX	LLONG_MAX
#endif
/* c99 has LLONG_MIN instead of LONG_LONG_MIN */
#ifndef LONG_LONG_MIN
#define LONG_LONG_MIN	LLONG_MIN
#endif

/*
 *  Instantiate an error checking wrapper for strtoll (long long)
 */

rtems_status_code rtems_string_to_long_long (
  const char *s,
  long long *n,
  char **endptr,
  int base
)
{
  long long result;
  char *end;

  if ( !n )
    return RTEMS_INVALID_ADDRESS;

  errno = 0;
  *n = 0;

  result = strtoll( s, &end, base );

  if ( endptr )
    *endptr = end;

  if ( end == s )
    return RTEMS_NOT_DEFINED;

  if ( ( errno == ERANGE ) && 
    (( result == 0 ) || ( result == LONG_LONG_MAX ) || ( result == LONG_LONG_MIN )))
      return RTEMS_INVALID_NUMBER;

  *n = result;

  return RTEMS_SUCCESSFUL;
}
