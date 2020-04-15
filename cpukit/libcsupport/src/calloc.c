/**
 *  @file
 *
 *  @brief Allocate Space for Array in Memory
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE_CALLOC)
#include <stdlib.h>
#include <string.h>
#include <rtems/score/basedefs.h>

void *calloc(
  size_t nelem,
  size_t elsize
)
{
  void   *cptr;
  size_t  length;

  length = nelem * elsize;
  cptr = malloc( length );
  RTEMS_OBFUSCATE_VARIABLE( cptr );
  if ( RTEMS_PREDICT_FALSE( cptr == NULL ) ) {
    return cptr;
  }

  return memset( cptr, 0, length );
}
#endif
