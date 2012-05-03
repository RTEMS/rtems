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

/* c99 has ULLONG_MAX instead of ULONG_LONG_MAX */
#ifndef ULONG_LONG_MAX
#define ULONG_LONG_MAX	ULLONG_MAX
#endif

/*
 *  Instantiate an error checking wrapper for strtoull (unsigned long long)
 */

rtems_status_code rtems_string_to_unsigned_long_long (
  const char *s,
  unsigned long long *n,
  char **endptr,
  int base
)
{
  unsigned long long result;
  char *end;

  if ( !n )
    return RTEMS_INVALID_ADDRESS;

  errno = 0;
  *n = 0;

  result = strtoull( s, &end, base );

  if ( endptr )
    *endptr = end;

  if ( end == s )
    return RTEMS_NOT_DEFINED;

  if ( ( errno == ERANGE ) && 
    (( result == 0 ) || ( result == ULONG_LONG_MAX )))
      return RTEMS_INVALID_NUMBER;

  *n = result;

  return RTEMS_SUCCESSFUL;
}
