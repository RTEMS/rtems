/*
 *  calloc()
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE_CALLOC)
#include "malloc_p.h"
#include <stdlib.h>

void *calloc(
  size_t nelem,
  size_t elsize
)
{
  register char *cptr;
  size_t length;

  MSBUMP(calloc_calls, 1);

  length = nelem * elsize;
  cptr = malloc( length );
  if ( cptr )
    memset( cptr, '\0', length );

  MSBUMP(malloc_calls, (uint32_t) -1);   /* subtract off the malloc */

  return cptr;
}
#endif
