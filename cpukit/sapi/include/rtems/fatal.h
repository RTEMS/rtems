/**
 * @file rtems/fatal.h
 *
 * This include file contains constants and prototypes related
 * to the Fatal Error Manager.  This manager processes all fatal or
 * irrecoverable errors.
 *
 * This manager provides directives to:
 *
 *   - announce a fatal error has occurred
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_FATAL_H
#define _RTEMS_FATAL_H

#include <rtems/score/basedefs.h> /* RTEMS_COMPILER_NO_RETURN_ATTRIBUTE */

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

void rtems_fatal_error_occurred(
  uint32_t   the_error
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
