/*
 *  Workspace Handler
 *
 *  NOTE:
 *
 *  This file only exists to contain the one function which cannot
 *  be written as a macro when "static inlines" are not used.
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

#include <rtems/system.h>
#include <rtems/wkspace.h>
#include <rtems/fatal.h>

#ifndef USE_INLINES

/*PAGE
 *
 *  _Workspace_Allocate_or_fatal_error
 *
 */

void *_Workspace_Allocate_or_fatal_error(
  unsigned32   size
)
{
  void        *memory;

  memory = _Workspace_Allocate( size );

  if ( memory == NULL )
    rtems_fatal_error_occurred( RTEMS_UNSATISFIED );

  return memory;
}

#endif   /* USE_INLINES */

