/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 *
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

#include <time.h>

int test( void );

int test( void )
{
  char      *buffer_pointer;
  struct tm  timestruct;
  char       buffer[ 80 ];

  buffer_pointer = asctime_r( &timestruct, buffer );

  return (buffer_pointer != NULL);
}
