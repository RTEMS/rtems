/*
 *  COPYRIGHT (c) 2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#include <math.h>

/*
 * This file is designed to be included multiple times to instantiate
 * it and should NOT be protected against multiple inclusions.
 */

#if !defined(STRING_TO_FLOAT) && !defined(STRING_TO_INTEGER)
  #error "Neither STRING_TO_FLOAT nor STRING_TO_INTEGER defined"
#endif

#if defined(STRING_TO_FLOAT) && defined(STRING_TO_INTEGER)
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
#elif defined(STRING_TO_INTEGER)
  #define ZERO 0
#endif

bool STRING_TO_NAME (
  const char      *s,
  STRING_TO_TYPE  *n,
  char           **endptr
  #if defined(STRING_TO_INTEGER)
    ,
    int              base
  #endif
)
{
  STRING_TO_TYPE  result;
  char           *end;

  if ( !n )
    return false;

  errno = 0;
  *n    = 0;

  #ifdef STRING_TO_FLOAT
    result = STRING_TO_METHOD( s, &end );
  #elif defined(STRING_TO_INTEGER)
    result = STRING_TO_METHOD( s, &end, base );
  #endif

  /* If the user wants the end pointer back, then return it. */
  if ( endptr )
    *endptr = end; 

  /* nothing was converted */
  if ( end == s )
    return false;

  /* there was a conversion error */
  if ( (result == ZERO) && errno )
    return false;

  #ifdef STRING_TO_MAX
    /* there was an overflow */
    if ( (result == STRING_TO_MAX) && (errno == ERANGE))
      return false;
  #endif

  #ifdef STRING_TO_MIN
    /* there was an underflow */
    if ( (result == STRING_TO_MIN) && (errno == ERANGE))
      return false;
  #endif

  *n = (STRING_TO_TYPE) result;
  return true;
}

