/*  address.h
 *
 *  This include file contains the information required to manipulate
 *  physical addresses.
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

#ifndef __RTEMS_ADDRESSES_h
#define __RTEMS_ADDRESSES_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _Addresses_Add_offset
 *
 *  DESCRIPTION:
 *
 *  This function is used to add an offset to a base address.
 *  It returns the resulting address.  This address is typically
 *  converted to an access type before being used further.
 */

STATIC INLINE void *_Addresses_Add_offset (
  void       *base,
  unsigned32  offset
);

/*
 *  _Addresses_Subtract_offset
 *
 *  DESCRIPTION:
 *
 *  This function is used to subtract an offset from a base
 *  address.  It returns the resulting address.  This address is
 *  typically converted to an access type before being used further.
 */

STATIC INLINE void *_Addresses_Subtract_offset(
  void       *base,
  unsigned32  offset
);

/*
 *  _Addresses_Subtract
 *
 *  DESCRIPTION:
 *
 *  This function is used to subtract two addresses.  It returns the
 *  resulting offset.
 */

STATIC INLINE unsigned32 _Addresses_Subtract (
  void *left,
  void *right
);

/*
 *  _Addresses_Is_aligned
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the given address is correctly
 *  aligned for this processor and FALSE otherwise.  Proper alignment
 *  is based on correctness and efficiency.
 */

STATIC INLINE boolean _Addresses_Is_aligned (
  void *address
);

/*
 *  _Addresses_Is_in_range
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the given address is within the
 *  memory range specified and FALSE otherwise.  base is the address
 *  of the first byte in the memory range and limit is the address
 *  of the last byte in the memory range.  The base address is
 *  assumed to be lower than the limit address.
 */

STATIC INLINE boolean _Addresses_Is_in_range (
  void *address,
  void *base,
  void *limit
);

#include <rtems/score/address.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
