/**
 *  @file
 *
 *  @brief Test for a Terminal Device
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE__ISATTY_R)

 /**
 *  This is the Newlib dependent reentrant version of isatty().
 */
#include <unistd.h>
#include <reent.h>
#include <sys/stat.h>

int _isatty_r(
  struct _reent *ptr,
  int            fd
)
{
  (void) ptr;
  return isatty( fd );
}
#endif
