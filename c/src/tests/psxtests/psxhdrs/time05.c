/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <time.h>
 
void test( void )
{
  size_t    length;
  size_t    max_length;
  char      buffer[ 80 ];
  struct tm timestruct;
  
  max_length = sizeof( buffer );

  length = strftime( buffer, max_length, "%A", &timestruct );
}
