/**
 * @file
 *
 * @ingroup libmisc_conv_help Conversion Helpers
 *
 * @brief Convert String to Double (with validation)
 */

/*
 *  COPYRIGHT (c) 2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2011  Ralf Corsepius, Ulm, Germany.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdlib.h>
#include <math.h>

#include <rtems/stringto.h>

/*
 *  Instantiate an error checking wrapper for strtod (double)
 */

rtems_status_code rtems_string_to_double (
  const char *s,
  double *n,
  char **endptr
)
{
  double result;
  char *end;

  if ( !n )
    return RTEMS_INVALID_ADDRESS;

  errno = 0;
  *n = 0;

  result = strtod( s, &end );

  if ( endptr )
    *endptr = end;

  if ( end == s )
    return RTEMS_NOT_DEFINED;

  if ( ( errno == ERANGE ) &&
    (( result == 0 ) || ( result == HUGE_VAL ) || ( result == -HUGE_VAL )))
      return RTEMS_INVALID_NUMBER;

  *n = result;

  return RTEMS_SUCCESSFUL;
}
