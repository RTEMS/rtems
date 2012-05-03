/**
 * @file rtems/rtems/status.inl
 *
 *  This include file contains the implementations of the inlined
 *  routines for the status package.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_STATUS_H
# error "Never use <rtems/rtems/status.inl> directly; include <rtems/rtems/status.h> instead."
#endif

#ifndef _RTEMS_RTEMS_STATUS_INL
#define _RTEMS_RTEMS_STATUS_INL

#include <rtems/score/basedefs.h>

/**
 *  @addtogroup ClassicStatus
 *  @{
 */

/**
 *  @brief rtems_is_status_successful
 *
 *  This function returns TRUE if the status code is equal to RTEMS_SUCCESSFUL,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_is_status_successful(
  rtems_status_code code
)
{
  return (code == RTEMS_SUCCESSFUL);
}

/**
 *  @brief rtems_are_statuses_equal
 *
 *  This function returns TRUE if the status code1 is equal to code2,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_are_statuses_equal(
  rtems_status_code code1,
  rtems_status_code code2
)
{
   return (code1 == code2);
}

/**@}*/

#endif
/* end of include file */
