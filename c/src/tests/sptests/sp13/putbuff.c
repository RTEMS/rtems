/*  Put_buffer
 *
 *  This test routine prints the given buffer.
 *  buffer.
 *
 *  Input parameters:
 *    buffer - pointer to message buffer to be printer
 *
 *  Output parameters:  NONE
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

#include "system.h"

#define MESSAGE_SIZE (4)

void Put_buffer(
  long *buffer
)
{
  int i;
/*
  printf( "%16s", (char *)buffer );
*/
  for ( i=0 ; i< MESSAGE_SIZE ; i++ ) {
    printf( "%c%c%c%c", (char) (buffer[i] >> 24),
                        (char) (buffer[i] >> 16 & 0xff),
                        (char) (buffer[i] >> 8 & 0xff),
                        (char) (buffer[i] >> 0 & 0xff) );
  }

}
