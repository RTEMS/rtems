/*  Fill_buffer
 *
 *  This test routine copies a given source string to a given destination
 *  buffer.
 *
 *  Input parameters:
 *    source - pointer to string to be copied
 *    buffer - pointer to message buffer to be filled
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

#define MESSAGE_SIZE (sizeof(long) * 4)

void Fill_buffer(
  char *source,
  long *buffer
)
{
  char *p;
  int i;
/*
  memcpy( buffer, source, 16 );
*/
  p = source;
  for ( i=0 ; i<MESSAGE_SIZE ; i++ ) {
    buffer[i] = *p++;
    buffer[i] <<= 8;
    buffer[i] |= *p++;
    buffer[i] <<= 8;
    buffer[i] |= *p++;
    buffer[i] <<= 8;
    buffer[i] |= *p++;
  }
}
