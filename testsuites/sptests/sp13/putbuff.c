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
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include "system.h"

#define BUFFER_LENGTH 16

void Put_buffer(
  long *buffer
)
{
  printf( "%16s", (char *)buffer );
}
