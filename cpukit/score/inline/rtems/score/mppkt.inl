/** 
 *  @file  rtems/score/mppkt.inl
 *
 *  This package is the implementation of the Packet Handler
 *  routines which are inlined.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_MPPKT_H
# error "Never use <rtems/score/mppkt.inl> directly; include <rtems/score/mppkt.h> instead."
#endif

#ifndef _RTEMS_SCORE_MPPKT_INL
#define _RTEMS_SCORE_MPPKT_INL

/**
 *  @addtogroup ScoreMPPacket 
 *  @{
 */

/**
 *  This function returns true if the the_packet_class is valid,
 *  and false otherwise.
 *
 *  @note Check for lower bounds (MP_PACKET_CLASSES_FIRST ) is unnecessary
 *        because this enum starts at lower bound of zero.
 */

RTEMS_INLINE_ROUTINE bool _Mp_packet_Is_valid_packet_class (
  MP_packet_Classes the_packet_class
)
{
  return ( the_packet_class <= MP_PACKET_CLASSES_LAST );
}

/**
 *  This function returns true if the the_packet_class is null,
 *  and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Mp_packet_Is_null (
  MP_packet_Prefix   *the_packet
)
{
  return the_packet == NULL;
}

/**@}*/

#endif
/* end of include file */
