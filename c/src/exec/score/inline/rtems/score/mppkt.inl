/*  inline/mppkt.inl
 *
 *  This package is the implementation of the Packet Handler
 *  routines which are inlined.
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

#ifndef __INLINE_MP_PACKET_inl
#define __INLINE_MP_PACKET_inl

/*PAGE
 *
 *  _Mp_packet_Is_valid_packet_class
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the the_packet_class is valid,
 *  and FALSE otherwise.
 *
 *  NOTE: Check for lower bounds (MP_PACKET_CLASSES_FIRST ) is unnecessary
 *        because this enum starts at lower bound of zero.
 */

RTEMS_INLINE_ROUTINE boolean _Mp_packet_Is_valid_packet_class (
  MP_packet_Classes the_packet_class
)
{
  return ( the_packet_class <= MP_PACKET_CLASSES_LAST );
}

/*PAGE
 *
 *  _Mp_packet_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the the_packet_class is null,
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Mp_packet_Is_null (
  MP_packet_Prefix   *the_packet
)
{
  return the_packet == NULL;
}

#endif
/* end of include file */
