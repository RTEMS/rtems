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
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include "system.h"

void Fill_buffer(
  char *source,
  long *buffer
)
{
  memcpy( buffer, source, 16 );
}
