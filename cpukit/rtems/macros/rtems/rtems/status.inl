/*  macros/status.h
 *
 *  This include file contains the implementations of the inlined
 *  routines for the status package.
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

/*
 *  _Status_Is_proxy_blocking
 *
 */

#define _Status_Is_proxy_blocking( _code ) \
  ( (_code) == RTEMS_PROXY_BLOCKING )

#endif
/* end of include file */
