/**
 * @file
 *
 * @ingroup RTEMSScorePriority
 *
 * @brief Inlined Routines in the Priority Handler Bit Map Implementation
 *
 * This file contains the static inline implementation of all inlined
 * routines in the Priority Handler bit map implementation
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_PRIORITYBITMAPIMPL_H
#define _RTEMS_SCORE_PRIORITYBITMAPIMPL_H

#include <rtems/score/prioritybitmap.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScorePriority
 *
 * @{
 */

/**
 *  This table is used by the generic bitfield routines to perform
 *  a highly optimized bit scan without the use of special CPU
 *  instructions.
 */
extern const unsigned char _Bitfield_Leading_zeros[256];

/**
 * @brief Returns the bit number of the first bit set in the specified value.
 *
 * The correspondence between the bit number and actual bit position is CPU
 * architecture dependent.  The search for the first bit set may run from most
 * to least significant bit or vice-versa.
 *
 * @param value The value to bit scan.
 *
 * @return The bit number of the first bit set.
 *
 * @see _Priority_Bits_index() and _Priority_Mask().
 */
RTEMS_INLINE_ROUTINE unsigned int _Bitfield_Find_first_bit(
  unsigned int value
)
{
  unsigned int bit_number;

#if ( CPU_USE_GENERIC_BITFIELD_CODE == FALSE )
  _CPU_Bitfield_Find_first_bit( value, bit_number );
#elif defined(__GNUC__)
  bit_number = (unsigned int) __builtin_clz( value )
    - __SIZEOF_INT__ * __CHAR_BIT__ + 16;
#else
  if ( value < 0x100 ) {
    bit_number = _Bitfield_Leading_zeros[ value ] + 8;
  } else { \
    bit_number = _Bitfield_Leading_zeros[ value >> 8 ];
  }
#endif

  return bit_number;
}

/**
 * @brief Returns the priority bit mask for the specified major or minor bit
 * number.
 *
 * @param bit_number The bit number for which we need a mask.
 *
 * @return The priority bit mask.
 */
RTEMS_INLINE_ROUTINE Priority_bit_map_Word _Priority_Mask(
  unsigned int bit_number
)
{
#if ( CPU_USE_GENERIC_BITFIELD_CODE == FALSE )
  return _CPU_Priority_Mask( bit_number );
#else
  return (Priority_bit_map_Word) ( 0x8000u >> bit_number );
#endif
}

/**
 * @brief Returns the bit index position for the specified major or minor bit
 * number.
 *
 * @param bit_number The bit number for which we need an index.
 *
 * @return The corresponding array index into the priority bit map.
 */
RTEMS_INLINE_ROUTINE unsigned int _Priority_Bits_index(
  unsigned int bit_number
)
{
#if ( CPU_USE_GENERIC_BITFIELD_CODE == FALSE )
  return _CPU_Priority_bits_index( bit_number );
#else
  return bit_number;
#endif
}

/**
 * @brief Returns the major portion of the_priority.
 *
 * @param the_priority The priority of which we want the major portion.
 *
 * @return The major portion of the priority.
 */
RTEMS_INLINE_ROUTINE unsigned int _Priority_Major( unsigned int the_priority )
{
  return the_priority / 16;
}

/**
 * @brief Returns the minor portion of the_priority.
 *
 * @param the_priority The priority of which we want the minor portion.
 *
 * @return The minor portion of the priority.
 */
RTEMS_INLINE_ROUTINE unsigned int _Priority_Minor( unsigned int the_priority )
{
  return the_priority % 16;
}

/**
 * @brief Initializes a bit map.
 *
 * @param[out] bit_map The bit map to initialize.
 */
RTEMS_INLINE_ROUTINE void _Priority_bit_map_Initialize(
  Priority_bit_map_Control *bit_map
)
{
  memset( bit_map, 0, sizeof( *bit_map ) );
}

/**
 * @brief Adds Priority queue bit map information.
 *
 * Priority Queue implemented by bit map.
 *
 * @param[out] bit_map The bit map to be altered by @a bit_map_info.
 * @param bit_map_info The information with which to alter @a bit_map.
 */
RTEMS_INLINE_ROUTINE void _Priority_bit_map_Add (
  Priority_bit_map_Control     *bit_map,
  Priority_bit_map_Information *bit_map_info
)
{
  *bit_map_info->minor |= bit_map_info->ready_minor;
  bit_map->major_bit_map |= bit_map_info->ready_major;
}

/**
 * @brief Removes Priority queue bit map information.
 *
 * Priority Queue implemented by bit map.
 *
 * @param[out] bit_map The bit map to be altered by @a bit_map_info.
 * @param bit_map_info The information with which to alter @a bit_map.
 */
RTEMS_INLINE_ROUTINE void _Priority_bit_map_Remove (
  Priority_bit_map_Control     *bit_map,
  Priority_bit_map_Information *bit_map_info
)
{
  *bit_map_info->minor &= bit_map_info->block_minor;
  if ( *bit_map_info->minor == 0 )
    bit_map->major_bit_map &= bit_map_info->block_major;
}

/**
 * @brief Gets highest portion of Priority queue bit map.
 *
 * @param bit_map The bitmap to get the highest portion from.
 *
 * @return The highest portion of the bitmap.
 */
RTEMS_INLINE_ROUTINE unsigned int _Priority_bit_map_Get_highest(
  const Priority_bit_map_Control *bit_map
)
{
  unsigned int minor;
  unsigned int major;

  major = _Bitfield_Find_first_bit( bit_map->major_bit_map );
  minor = _Bitfield_Find_first_bit( bit_map->bit_map[ major ] );

  return (_Priority_Bits_index( major ) << 4) +
          _Priority_Bits_index( minor );
}

/**
 * @brief Checks if the Priority queue bit map is empty.
 *
 * @param bit_map The bit map of which to check if it is empty.
 *
 * @retval true The Priority queue bit map is empty
 * @retval false The Priority queue bit map is not empty.
 */
RTEMS_INLINE_ROUTINE bool _Priority_bit_map_Is_empty(
  const Priority_bit_map_Control *bit_map
)
{
  return bit_map->major_bit_map == 0;
}

/**
 * @brief Initializes the bit map information.
 *
 * @param bit_map The bit map for the initialization of the bit
 *      map information.
 * @param[out] bit_map_info The bit map information to initialize.
 * @param new_priority The new priority for the initialization
 *      of the bit map information.
 */
RTEMS_INLINE_ROUTINE void _Priority_bit_map_Initialize_information(
  Priority_bit_map_Control     *bit_map,
  Priority_bit_map_Information *bit_map_info,
  unsigned int                  new_priority
)
{
  unsigned int major;
  unsigned int minor;
  Priority_bit_map_Word mask;

  major = _Priority_Major( new_priority );
  minor = _Priority_Minor( new_priority );

  bit_map_info->minor = &bit_map->bit_map[ _Priority_Bits_index( major ) ];

  mask = _Priority_Mask( major );
  bit_map_info->ready_major = mask;
  bit_map_info->block_major = (Priority_bit_map_Word) ~mask;

  mask = _Priority_Mask( minor );
  bit_map_info->ready_minor = mask;
  bit_map_info->block_minor = (Priority_bit_map_Word) ~mask;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
