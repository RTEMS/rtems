/*  inline/address.inl
 *
 *  This include file contains the bodies of the routines
 *  about addresses which are inlined.
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

#ifndef __INLINE_ADDRESSES_inl
#define __INLINE_ADDRESSES_inl

/*PAGE
 *
 *  _Addresses_Add_offset
 *
 */

STATIC INLINE void *_Addresses_Add_offset (
  void       *base,
  unsigned32  offset
)
{
  return (void *)((char *)base + offset);
}

/*PAGE
 *
 *  _Addresses_Subtract_offset
 *
 */

STATIC INLINE void *_Addresses_Subtract_offset (
  void       *base,
  unsigned32  offset
)
{
  return (void *)((char *)base - offset);
}

/*PAGE
 *
 *  _Addresses_Subtract
 *
 *  NOTE:  The cast of an address to an unsigned32 makes this code
 *         dependent on an addresses being thirty two bits.
 */

STATIC INLINE unsigned32 _Addresses_Subtract (
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
    return ( ( (unsigned32)address % CPU_ALIGNMENT ) == 0 );
}

/*PAGE
 *
 *  _Addresses_Is_in_range
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
