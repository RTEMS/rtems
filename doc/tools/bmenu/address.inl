/*  inline/address.inl
 *
 *  This include file contains the bodies of the routines
 *  about addresses which are inlined.
 *
 *  COPYRIGHT (c) 1988-2002.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights reserved.
 *
 *  $Id$
 */

#ifndef __INLINE_ADDRESSES_inl
#define __INLINE_ADDRESSES_inl

#include <stddef.h>
#include <stdint.h>

/*PAGE
 *
 *  _Addresses_Add_offset
 *
 */

STATIC INLINE void *_Addresses_Add_offset ( 
  void       *base, 
  size_t      offset
)
{
  return (base + offset);
}

/*PAGE
 *
 *  _Addresses_Subtract_offset
 *
 */

STATIC INLINE void *_Addresses_Subtract_offset ( 
  void       *base, 
  size_t      offset
)
{
  return (base - offset);
}

/*PAGE
 *
 *  _Addresses_Add
 *
 *  NOTE:  The cast of an address to an unsigned32 makes this code
 *         dependent on an addresses being thirty two bits.
 */

STATIC INLINE void *_Addresses_Add ( 
  void *left, 
  void *right 
)
{
  return (left + (ptrdiff_t) right);
}

/*PAGE
 *
 *  _Addresses_Subtract
 *
 *  NOTE:  The cast of an address to an unsigned32 makes this code
 *         dependent on an addresses being thirty two bits.
 */

STATIC INLINE ptrdiff_t _Addresses_Subtract ( 
  void *left, 
  void *right 
)
{
  return (left - right);
}

/*PAGE
 *
 *  _Addresses_Is_aligned
 *
 */

STATIC INLINE boolean _Addresses_Is_aligned ( 
  void *address
)
{
    return ( ( (intptr_t)address % 4 ) == 0 );
}

/*PAGE
 *
 *  _Addresses_Is_aligned
 *
 */

STATIC INLINE boolean _Addresses_Is_in_range ( 
  void *address, 
  void *base,
  void *limit 
)
{
  return ( address >= base && address <= limit );
}

#endif
/* end of include file */
