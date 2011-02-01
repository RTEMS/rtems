/*
 *  COPYRIGHT (c) 2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2011  Ralf Corsépius, Ulm, Germany.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdlib.h>
#include <limits.h>

#include <rtems/stringto.h>

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

  if ( (result == LONG_LONG_MAX) && (errno == ERANGE) )
    return RTEMS_INVALID_NUMBER;

  if ( (result == LONG_LONG_MIN) && (errno == ERANGE) )
    return RTEMS_INVALID_NUMBER;

  *n = result;

  return RTEMS_SUCCESSFUL;
}
