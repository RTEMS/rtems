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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "system.h"

void Put_buffer(
  long *buffer
)
{
  printf( "%16s", (char *)buffer );
}
