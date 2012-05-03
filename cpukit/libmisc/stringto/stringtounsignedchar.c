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

/*
 *  Instantiate an error checking wrapper for strtoul (unsigned char)
 */

rtems_status_code rtems_string_to_unsigned_char (
  const char *s,
  unsigned char *n,
  char **endptr,
  int base
)
{
  unsigned long result;
  char *end;

  if ( !n )
    return RTEMS_INVALID_ADDRESS;

  errno = 0;
  *n = 0;

  result = strtoul( s, &end, base );

  if ( endptr )
    *endptr = end;

  if ( end == s )
    return RTEMS_NOT_DEFINED;

  if ( ( errno == ERANGE ) && 
    (( result == 0 ) || ( result == ULONG_MAX )))
      return RTEMS_INVALID_NUMBER;

#if (UCHAR_MAX < ULONG_MAX)
  if ( result > UCHAR_MAX ) {
    errno = ERANGE;
    return RTEMS_INVALID_NUMBER;
  }
#endif

  *n = result;

  return RTEMS_SUCCESSFUL;
}
