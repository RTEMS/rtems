/*  macros/mppkt.h
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
