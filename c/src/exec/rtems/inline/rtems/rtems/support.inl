/*  support.inl
 *
 *  This include file contains the static inline implementation of all
 *  of the inlined routines specific to the RTEMS API.
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
 
#ifndef __RTEMS_SUPPORT_inl
#define __RTEMS_SUPPORT_inl

/*PAGE
 *
 *  rtems_is_name_valid
 *
 */

STATIC INLINE rtems_boolean rtems_is_name_valid (
  rtems_name name
)
{
  return ( name != 0 );
}

/*PAGE
 *
 *  rtems_name_to_characters
 *
 */

STATIC INLINE void rtems_name_to_characters(
  rtems_name    name,
  char         *c1,
  char         *c2,
  char         *c3,
  char         *c4
)
{
  *c1 = (name >> 24) & 0xff;
  *c2 = (name >> 16) & 0xff;
  *c3 = (name >> 8) & 0xff;
  *c4 =  name & 0xff;
}

#endif
/* end of include file */
