/*  macros/status.h
 *
 *  This include file contains the implementations of the inlined
 *  routines for the status package.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __MACROS_STATUS_h
#define __MACROS_STATUS_h

/*PAGE
 *
 *  rtems_is_status_successful
 *
 */

#define rtems_is_status_successful( _code ) \
  ( (_code) == RTEMS_SUCCESSFUL )

/*PAGE
 *
 *  rtems_are_statuses_equal
 *
 */

#define rtems_are_statuses_equal( _code1, _code2 ) \
   ((_code1) == (_code2))

#endif
/* end of include file */
