/*
 * Copyright (c) 2016 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/assoc.h>

#include <stdbool.h>
#include <string.h>

static size_t space( size_t buffer_size, size_t len )
{
  if ( len < buffer_size ) {
    return buffer_size - len;
  } else {
    return 0;
  }
}

size_t rtems_assoc_32_to_string(
  uint32_t                   value,
  char                      *buffer,
  size_t                     buffer_size,
  const rtems_assoc_32_pair *pairs,
  size_t                     pair_count,
  const char                *separator,
  const char                *fallback
)
{
  size_t len;
  size_t i;

  len = 0;

  for ( i = 0; i < pair_count ; ++i ) {
    const rtems_assoc_32_pair *p;

    p = &pairs[ i ];

    if ( ( value & p->bits ) != 0 ) {
      if ( len > 0 ) {
        len += strlcpy( &buffer[ len ], separator, space( buffer_size, len ) );
      }

      len += strlcpy( &buffer[ len ], p->name, space( buffer_size, len ) );
    }
  }

  if ( len == 0 ) {
    len += strlcpy( buffer, fallback, buffer_size );
  }

  return len;
}
