/*
 *  chown() - POSIX 1003.1b 5.6.5 - Change Owner and Group of a File
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

#include <sys/stat.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>

#include "libio_.h"

int chown(
  const char *path,
  uid_t       owner,
  gid_t       group
)
{
  rtems_filesystem_location_info_t   loc;
  int                                result;

  if ( rtems_filesystem_evaluate_path( path, 0x00, &loc, TRUE ) )
    return -1;
  
  if ( !loc.ops->chown ) {
    if ( loc.ops->freenod )
      (*loc.ops->freenod)( &loc );
    set_errno_and_return_minus_one( ENOTSUP );
  }

  result = (*loc.ops->chown)( &loc, owner, group );

  if ( loc.ops->freenod )
    (*loc.ops->freenod)( &loc );
  
  return result;
}
