/*  inline/address.inl
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

#ifndef __INLINE_ADDRESSES_inl
#define __INLINE_ADDRESSES_inl

/*PAGE
 *
 *  _Addresses_Add_offset
 *
 *  DESCRIPTION:
 *
 *  This function is used to add an offset to a base address.
 *  It returns the resulting address.  This address is typically
 *  converted to an access type before being used further.
 */

RTEMS_INLINE_ROUTINE void *_Addresses_Add_offset (
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
 *  DESCRIPTION:
 *
 *  This function is used to subtract an offset from a base
 *  address.  It returns the resulting address.  This address is
 *  typically converted to an access type before being used further.
 */

RTEMS_INLINE_ROUTINE void *_Addresses_Subtract_offset (
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
 *  DESCRIPTION:
 *
 *  This function is used to subtract two addresses.  It returns the
 *  resulting offset.
 *
 *  NOTE:  The cast of an address to an unsigned32 makes this code
 *         dependent on an addresses being thirty two bits.
 */

RTEMS_INLINE_ROUTINE unsigned32 _Addresses_Subtract (
  void *left,
  void *right
)
{
  return ((char *) left - (char *) right);
}

/*PAGE
 *
 *  _Addresses_Is_aligned
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the given address is correctly
 *  aligned for this processor and FALSE otherwise.  Proper alignment
 *  is based on correctness and efficiency.
 */

RTEMS_INLINE_ROUTINE boolean _Addresses_Is_aligned (
  void *address
)
{
#if (CPU_ALIGNMENT == 0)
    return TRUE;
#elif defined(RTEMS_CPU_HAS_16_BIT_ADDRESSES)
    return ( ( (unsigned short)address % CPU_ALIGNMENT ) == 0 );
#else
    return ( ( (unsigned32)address % CPU_ALIGNMENT ) == 0 );
#endif
}

/*PAGE
 *
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

RTEMS_INLINE_ROUTINE boolean _Addresses_Is_in_range (
  void *address,
  void *base,
  void *limit
)
{
  return ( address >= base && address <= limit );
}

#endif
/* end of include file */
