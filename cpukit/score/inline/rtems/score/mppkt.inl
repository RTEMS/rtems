/*  inline/mppkt.inl
 *
 *  This package is the implementation of the Packet Handler
 *  routines which are inlined.
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
