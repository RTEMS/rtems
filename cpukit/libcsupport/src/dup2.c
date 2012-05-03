/*
 *  dup2() - POSIX 1003.1b 6.2.1 Duplicate an Open File Descriptor
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <fcntl.h>

#include <rtems/libio_.h>

int dup2(
  int fildes,
  int fildes2
)
{
  int          status;
  struct stat  buf;

  /*
   *  If fildes is not valid, then fildes2 should not be closed.
   */

  status = fstat( fildes, &buf );
  if ( status == -1 )
    return -1;

  /*
   *  If fildes2 is not valid, then we should not do anything either.
   */

  status = fstat( fildes2, &buf );
  if ( status == -1 )
    return -1;

  /*
   *  This fcntl handles everything else.
   */

  return fcntl( fildes, F_DUPFD, fildes2 );
}
