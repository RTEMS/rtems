/*  fatal.h
 *
 *  This include file contains constants and prototypes related
 *  to the Fatal Error Manager.  This manager processes all fatal or
 *  irrecoverable errors.
 *
 *  This manager provides directives to:
 *
 *     + announce a fatal error has occurred
 *
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

#ifndef __RTEMS_FATAL_h
#define __RTEMS_FATAL_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  rtems_fatal_error_occurred
 *
 *  DESCRIPTION:
 *
 *  This is the routine which implements the rtems_fatal_error_occurred
 *  directive.  It is invoked when the application or RTEMS
 *  determines that a fatal error has occurred.
 */

void volatile rtems_fatal_error_occurred(
  unsigned32 the_error
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
