/*  options.inl
 *
 *  This file contains the static inline implementation of the inlined
 *  routines from the Options Handler.
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

#ifndef __OPTIONS_inl
#define __OPTIONS_inl

/*PAGE
 *
 *  _Options_Is_no_wait
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the RTEMS_NO_WAIT option is enabled in
 *  option_set, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Options_Is_no_wait (
  rtems_option option_set
)
{
   return (option_set & RTEMS_NO_WAIT);
}

/*PAGE
 *
 *  _Options_Is_any
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the RTEMS_EVENT_ANY option is enabled in
 *  OPTION_SET, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Options_Is_any (
  rtems_option option_set
)
{
   return (option_set & RTEMS_EVENT_ANY);
}

#endif
/* end of include file */
