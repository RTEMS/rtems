/*  macros/mppkt.h
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

#ifndef __MACROS_MP_PACKET_h
#define __MACROS_MP_PACKET_h

/*PAGE
 *
 *  _Mp_packet_Is_valid_packet_class
 *
 *  NOTE: Check for lower bounds (MP_PACKET_CLASSES_FIRST ) is unnecessary
 *        because this enum starts at lower bound of zero.
 */

#define _Mp_packet_Is_valid_packet_class( _the_packet_class ) \
  ( (_the_packet_class) <= MP_PACKET_CLASSES_LAST )

/*PAGE
 *
 *  _Mp_packet_Is_null
 *
 */

#define _Mp_packet_Is_null ( _the_packet ) \
  ( (_the_packet) == NULL )

#endif
/* end of include file */
