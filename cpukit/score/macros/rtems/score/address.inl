/*  macros/address.h
 *
 *  This include file contains the bodies of the routines
 *  about addresses which are inlined.
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

#ifndef __MACROS_ADDRESSES_h
#define __MACROS_ADDRESSES_h

/*PAGE
 *
 *  _Addresses_Add_offset
 *
 */

#define _Addresses_Add_offset( _base, _offset ) \
  ((void *)((char *)(_base) + (_offset)))

/*PAGE
 *
 *  _Addresses_Subtract_offset
 *
 */

#define _Addresses_Subtract_offset( _base, _offset ) \
  ((void *)((char *)(_base) - (_offset)))

/*PAGE
 *
 *  _Addresses_Subtract
 *
 *  NOTE:  The cast of an address to an unsigned32 makes this code
 *         dependent on an addresses being thirty two bits.
 */

#define _Addresses_Subtract( _left, _right ) \
  ((void *)(_left) - (void *)(_right))

/*PAGE
 *
 *  _Addresses_Is_aligned
 *
 */

#if (CPU_ALIGNMENT == 0)
#define _Addresses_Is_aligned( _address ) \
    (TRUE)
#elif defined(RTEMS_CPU_HAS_16_BIT_ADDRESSES)
#define _Addresses_Is_aligned( _address ) \
    ( ( (unsigned short)(_address) % CPU_ALIGNMENT ) == 0 )
#else
#define _Addresses_Is_aligned( _address ) \
    ( ( (unsigned32)(_address) % CPU_ALIGNMENT ) == 0 )
#endif

/*PAGE
 *
 *  _Addresses_Is_in_range
 *
 */

#define _Addresses_Is_in_range( _address, _base, _limit ) \
  ( (_address) >= (_base) && (_address) <= (_limit) )

#endif
/* end of include file */
