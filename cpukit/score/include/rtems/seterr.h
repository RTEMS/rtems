/**
 *  @file  rtems/seterr.h
 *
 *  This file contains macros and definitions which ease the burden
 *  of consistently setting errno and returning -1.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SETERR_H
#define _RTEMS_SETERR_H

#include <errno.h>

/**
 *  This is a helper macro which will set the variable errno and return
 *  -1 to the caller.  This pattern is common to many POSIX methods.
 *
 *  @param[in] _error is the error code
 */
#define rtems_set_errno_and_return_minus_one( _error ) \
  do { errno = (_error); return -1; } while(0)

/**
 *  This is a helper macro which will set the variable errno and return
 *  -1 to the caller.  This pattern is common to many POSIX methods.
 *
 *  @param[in] _error is the error code
 *  @param[in] _cast is the type to which -1 must be cast
 *
 *  @note It is similar to @ref rtems_set_errno_and_return_minus_one but
 *        this -1 value is cast to something other than an int.
 */
#define rtems_set_errno_and_return_minus_one_cast( _error, _cast ) \
  do { errno = (_error); return (_cast) -1; } while(0)

#endif
/* end of include file */
