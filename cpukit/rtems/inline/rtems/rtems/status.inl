/*  inline/status.inl
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
