/*
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
 
#ifndef __POSIX_SET_ERRNO_h
#define __POSIX_SET_ERRNO_h
 
#define set_errno_and_return_minus_one( _error ) \
  { errno = (_error); return -1; }

#endif
/* end of include file */
