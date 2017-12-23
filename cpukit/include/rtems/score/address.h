/**
 *  @file  rtems/score/address.h
 *
 *  @brief Information Required to Manipulate Physical Addresses
 *
 *  This include file contains the information required to manipulate
 *  physical addresses.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ADDRESS_H
#define _RTEMS_SCORE_ADDRESS_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreAddress Address Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which abstracts address
 *  manipulation in a portable manner.
 */
/**@{*/

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

/**
 * @brief Align address to nearest multiple of alignment, rounding up.
 *
 * This function returns the given address aligned to the given alignment.
 * If the address already is aligned, or if alignment is 0, the address is
 * returned as is. The returned address is greater than or equal to the
 * given address.
 *
 * @param[in] address is the address to align.
 * @param[in] alignment is the boundary for alignment and must be a power of 2
 *
 * @return Returns the aligned address.
 */
RTEMS_INLINE_ROUTINE void *_Addresses_Align_up(
  void *address,
  size_t alignment
)
{
  uintptr_t mask = alignment - (uintptr_t)1;
  return (void*)(((uintptr_t)address + mask) & ~mask);
}

/**
 * @brief Align address to nearest multiple of alignment, truncating.
 *
 * This function returns the given address aligned to the given alignment.
 * If the address already is aligned, or if alignment is 0, the address is
 * returned as is. The returned address is less than or equal to the
 * given address.
 *
 * @param[in] address is the address to align.
 * @param[in] alignment is the boundary for alignment and must be a power of 2.
 *
 * @return Returns the aligned address.
 */
RTEMS_INLINE_ROUTINE void *_Addresses_Align_down(
  void *address,
  size_t alignment
)
{
  uintptr_t mask = alignment - (uintptr_t)1;
  return (void*)((uintptr_t)address & ~mask);
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
