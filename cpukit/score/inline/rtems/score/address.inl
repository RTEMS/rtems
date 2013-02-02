/**
 * @file
 *
 * @brief Inlined Routines Associated with Addresses
 *
 * This include file contains the bodies of the routines
 * about addresses which are inlined.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ADDRESS_H
# error "Never use <rtems/score/address.inl> directly; include <rtems/score/address.h> instead."
#endif

#ifndef _RTEMS_SCORE_ADDRESS_INL
#define _RTEMS_SCORE_ADDRESS_INL

#include <rtems/score/basedefs.h>

/**
 * @addtogroup ScoreAddress
 */
/**@{**/

/**
 * @brief Add offset to an address.
 *
 * This function is used to add an @a offset to a @a base address.
 * It returns the resulting address.  This address is typically
 * converted to an access type before being used further.
 *
 * @param[in] base is the base address.
 * @param[in] offset is the offset to add to @a base.
 *
 * @return This method returns the resulting address.
 */
#include <rtems/bspIo.h>
RTEMS_INLINE_ROUTINE void *_Addresses_Add_offset (
  const void *base,
  uintptr_t   offset
)
{
  return (void *)((uintptr_t)base + offset);
}

/**
 * @brief Subtract offset from offset.
 *
 * This function is used to subtract an @a offset from a @a base
 * address.  It returns the resulting address.  This address is
 * typically converted to an access type before being used further.
 *
 * @param[in] base is the base address.
 * @param[in] offset is the offset to subtract to @a base.
 *
 * @return This method returns the resulting address.
 */

RTEMS_INLINE_ROUTINE void *_Addresses_Subtract_offset (
  const void *base,
  uintptr_t   offset
)
{
  return (void *)((uintptr_t)base - offset);
}

/**
 * @brief Subtract two offsets.
 *
 * This function is used to subtract two addresses.  It returns the
 * resulting offset.
 *
 * @param[in] left is the address on the left hand side of the subtraction.
 * @param[in] right is the address on the right hand side of the subtraction.
 *
 * @return This method returns the resulting address.
 *
 * @note  The cast of an address to an uint32_t makes this code
 *        dependent on an addresses being thirty two bits.
 */
RTEMS_INLINE_ROUTINE int32_t _Addresses_Subtract (
  const void *left,
  const void *right
)
{
  return (int32_t) ((const char *) left - (const char *) right);
}

/**
 * @brief Is address aligned.
 *
 * This function returns true if the given address is correctly
 * aligned for this processor and false otherwise.  Proper alignment
 * is based on correctness and efficiency.
 *
 * @param[in] address is the address being checked for alignment.
 *
 * @retval true The @a address is aligned.
 * @retval false The @a address is not aligned.
 */
RTEMS_INLINE_ROUTINE bool _Addresses_Is_aligned (
  const void *address
)
{
#if (CPU_ALIGNMENT == 0)
    return true;
#else
    return (((uintptr_t)address % CPU_ALIGNMENT) == 0);
#endif
}

/**
 * @brief Is address in range.
 *
 * This function returns true if the given address is within the
 * memory range specified and false otherwise.  base is the address
 * of the first byte in the memory range and limit is the address
 * of the last byte in the memory range.  The base address is
 * assumed to be lower than the limit address.
 *
 * @param[in] address is the address to check.
 * @param[in] base is the lowest address of the range to check against.
 * @param[in] limit is the highest address of the range to check against.
 *
 * @retval true The @a address is within the memory range specified
 * @retval false The @a address is not within the memory range specified.
 */
RTEMS_INLINE_ROUTINE bool _Addresses_Is_in_range (
  const void *address,
  const void *base,
  const void *limit
)
{
  return (address >= base && address <= limit);
}

/** @} */

#endif
/* end of include file */
