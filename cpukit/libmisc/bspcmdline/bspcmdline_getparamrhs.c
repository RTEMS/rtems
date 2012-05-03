/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <rtems/bspcmdline.h>

const char *rtems_bsp_cmdline_get_param_rhs(
  const char *name,
  char       *value,
  size_t      length
)
{
  const char *p;
  const char *rhs;
  char       *d;

  p = rtems_bsp_cmdline_get_param( name, value, length );
  if ( !p )
    return NULL;

  rhs = &p[strlen(name)];
  if ( *rhs != '=' )
    return NULL;

  rhs++;
  if ( *rhs == '\"' )
    rhs++;
  for ( d=value ; *rhs ; )
    *d++ = *rhs++;
  if ( *(d-1) == '\"' )
    d--;
  *d = '\0';

  return value;
}
