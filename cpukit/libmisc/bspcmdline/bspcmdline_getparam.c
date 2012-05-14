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

#include <rtems/bspcmdline.h>

static void copy_string(
  const char *start,
  char       *value,
  size_t      length
)
{
  int         i;
  int         quotes;
  const char *p = start;

  quotes=0;
  for (i=0 ; *p && i<length-1; ) {
    if ( *p == '\"' ) {
      quotes++;
    } else if ( ((quotes % 2) == 0) && *p == ' ' )
      break;
    value[i++] = *p++;
    value[i] = '\0';
  }

}

const char *rtems_bsp_cmdline_get_param(
  const char *name,
  char       *value,
  size_t      length
)
{
  const char *p;

  if ( !name )
    return NULL;

  if ( !value )
    return NULL;

  if ( !length )
    return NULL;

  value[0] = '\0';

  p = rtems_bsp_cmdline_get_param_raw( name );

  if ( !p )
    return NULL;

  copy_string( p, value, length );

  return value;
}
