/** 
 *  @file  score/src/timespecisvalid.c
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/timespec.h>
#include <rtems/score/tod.h>

boolean _Timespec_Is_valid(
  const struct timespec *time
)
{
  if ( !time )
    return FALSE;

  if ( time->tv_sec < 0 )
    return FALSE;

  if ( time->tv_nsec < 0 )
    return FALSE;

  if ( time->tv_nsec >= TOD_NANOSECONDS_PER_SECOND )
    return FALSE;

  return TRUE;
}
