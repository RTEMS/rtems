/*  inline/status.inl
 *
 *  This include file contains the implementations of the inlined
 *  routines for the status package.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __INLINE_STATUS_inl
#define __INLINE_STATUS_inl

/*PAGE
 *
 *  rtems_is_status_successful
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the status code is equal to RTEMS_SUCCESSFUL,
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean rtems_is_status_successful(
  rtems_status_code code
)
{
  return (code == RTEMS_SUCCESSFUL);
}

/*PAGE
 *
 *  rtems_are_statuses_equal
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the status code1 is equal to code2,
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean rtems_are_statuses_equal(
  rtems_status_code code1,
  rtems_status_code code2
)
{
   return (code1 == code2);
}

#endif
/* end of include file */
