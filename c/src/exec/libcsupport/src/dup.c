/*
 *  dup() - POSIX 1003.1b 6.2.1 Duplicate an Open File Descriptor
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <unistd.h>
#include <fcntl.h>

int dup(
  int fildes
)
{
  return fcntl( fildes, F_DUPFD, 0 );
}
