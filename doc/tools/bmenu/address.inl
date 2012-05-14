/*  inline/address.inl
 *
 *  This include file contains the bodies of the routines
 *  about addresses which are inlined.
 *
 *  COPYRIGHT (c) 1988-2002.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights reserved.
 */

#ifndef __INLINE_ADDRESSES_inl
#define __INLINE_ADDRESSES_inl

#include <stddef.h>

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

#endif
/* end of include file */
