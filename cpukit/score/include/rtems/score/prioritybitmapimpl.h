/**
 * @file
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_PRIORITYBITMAPIMPL_H
#define _RTEMS_SCORE_PRIORITYBITMAPIMPL_H

#include <rtems/score/prioritybitmap.h>
#include <rtems/score/priority.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScorePriority
 */
/**@{**/

/*
 * The Priority_bit_map_Control variables are instantiated only
 * if using the bit map handler.
 */

/**
 *  Each sixteen bit entry in this array is associated with one of
 *  the sixteen entries in the Priority Bit map.
 */
extern volatile Priority_bit_map_Control _Priority_Major_bit_map;

/** Each bit in the Priority Bitmap indicates whether or not there are
 *  threads ready at a particular priority.  The mapping of
 *  individual priority levels to particular bits is processor
 *  dependent as is the value of each bit used to indicate that
 *  threads are ready at that priority.
 */
extern Priority_bit_map_Control
               _Priority_Bit_map[16] CPU_STRUCTURE_ALIGNMENT;

#if ( CPU_USE_GENERIC_BITFIELD_DATA == TRUE )

/**
 *  This table is used by the generic bitfield routines to perform
 *  a highly optimized bit scan without the use of special CPU
 *  instructions.
 */
extern const unsigned char __log2table[256];

#endif

/**
 *  @brief Gets the @a _bit_number of the first bit set in the specified value.
 *
 *  This routine returns the @a _bit_number of the first bit set
 *  in the specified value.  The correspondence between @a _bit_number
 *  and actual bit position is processor dependent.  The search for
 *  the first bit set may run from most to least significant bit
 *  or vice-versa.
 *
 *  @param[in] _value is the value to bit scan.
 *  @param[in] _bit_number is the position of the first bit set.
 *
 *  @note This routine is used when the executing thread is removed
 *  from the ready state and, as a result, its performance has a
 *  significant impact on the performance of the executive as a whole.
 *
 *  @note This routine must be a macro because if a CPU specific version
 *  is used it will most likely use inline assembly.
 */
#if ( CPU_USE_GENERIC_BITFIELD_CODE == FALSE )
#define _Bitfield_Find_first_bit( _value, _bit_number ) \
        _CPU_Bitfield_Find_first_bit( _value, _bit_number )
#else
#define _Bitfield_Find_first_bit( _value, _bit_number ) \
  { \
    register uint32_t   __value = (uint32_t) (_value); \
    register const unsigned char *__p = __log2table; \
    \
    if ( __value < 0x100 ) \
      (_bit_number) = (Priority_bit_map_Control)( __p[ __value ] + 8 );  \
    else \
      (_bit_number) = (Priority_bit_map_Control)( __p[ __value >> 8 ] ); \
  }
#endif

#if ( CPU_USE_GENERIC_BITFIELD_CODE == FALSE )
/**
 *  This method returns the priority bit mask for the specified major
 *  or minor bit number.
 *
 *  @param[in] _bit_number is the bit number for which we need a mask
 *
 *  @retval the priority bit mask
 *
 *  @note This may simply be a pass through to a CPU dependent implementation.
 */
#define _Priority_Mask( _bit_number ) \
  _CPU_Priority_Mask( _bit_number )
#endif

#if ( CPU_USE_GENERIC_BITFIELD_CODE == FALSE )
/**
 *  This method returns the bit index position for the specified priority.
 *
 *  @param[in] _priority is the priority for which we need the index.
 *
 *  @retval This method returns the array index into the priority bit map.
 *
 *  @note This may simply be a pass through to a CPU dependent implementation.
 */
#define _Priority_Bits_index( _priority ) \
  _CPU_Priority_bits_index( _priority )
#endif

/**
 * This function returns the major portion of the_priority.
 */

RTEMS_INLINE_ROUTINE Priority_bit_map_Control   _Priority_Major (
  Priority_Control the_priority
)
{
  return (Priority_bit_map_Control)( the_priority / 16 );
}

/**
 * This function returns the minor portion of the_priority.
 */

RTEMS_INLINE_ROUTINE Priority_bit_map_Control   _Priority_Minor (
  Priority_Control the_priority
)
{
  return (Priority_bit_map_Control)( the_priority % 16 );
}

#if ( CPU_USE_GENERIC_BITFIELD_CODE == TRUE )

/**
 * This function returns the mask associated with the major or minor
 * number passed to it.
 */

RTEMS_INLINE_ROUTINE Priority_bit_map_Control   _Priority_Mask (
  uint32_t   bit_number
)
{
  return (Priority_bit_map_Control)(0x8000u >> bit_number);
}

/**
 * This function returns the mask bit inverted.
 */

RTEMS_INLINE_ROUTINE Priority_bit_map_Control   _Priority_Mask_invert (
  uint32_t   mask
)
{
  return (Priority_bit_map_Control)(~mask);
}

/**
 * This function translates the bit numbers returned by the bit scan
 * of a priority bit field into something suitable for use as
 * a major or minor component of a priority.
 */

RTEMS_INLINE_ROUTINE uint32_t   _Priority_Bits_index (
  uint32_t   bit_number
)
{
  return bit_number;
}

#endif

/**
 * Priority Queue implemented by bit map
 */

/**
 *  This is the minor bit map.
 */
extern Priority_bit_map_Control _Priority_Bit_map[16] CPU_STRUCTURE_ALIGNMENT;

/**
 * This routine uses the_priority_map to update the priority
 * bit maps to indicate that a thread has been readied.
 */

RTEMS_INLINE_ROUTINE void _Priority_bit_map_Add (
  Priority_bit_map_Information *the_priority_map
)
{
  *the_priority_map->minor |= the_priority_map->ready_minor;
  _Priority_Major_bit_map  |= the_priority_map->ready_major;
}

/**
 * This routine uses the_priority_map to update the priority
 * bit maps to indicate that a thread has been removed from the
 * ready state.
 */

RTEMS_INLINE_ROUTINE void _Priority_bit_map_Remove (
  Priority_bit_map_Information *the_priority_map
)
{
  *the_priority_map->minor &= the_priority_map->block_minor;
  if ( *the_priority_map->minor == 0 )
    _Priority_Major_bit_map &= the_priority_map->block_major;
}

/**
 * This function returns the priority of the highest priority
 * ready thread.
 */

RTEMS_INLINE_ROUTINE Priority_Control _Priority_bit_map_Get_highest( void )
{
  Priority_bit_map_Control minor;
  Priority_bit_map_Control major;

  _Bitfield_Find_first_bit( _Priority_Major_bit_map, major );
  _Bitfield_Find_first_bit( _Priority_Bit_map[major], minor );

  return (_Priority_Bits_index( major ) << 4) +
          _Priority_Bits_index( minor );
}

RTEMS_INLINE_ROUTINE bool _Priority_bit_map_Is_empty( void )
{
  return _Priority_Major_bit_map == 0;
}

/**
 * This routine initializes the_priority_map so that it
 * contains the information necessary to manage a thread
 * at new_priority.
 */

RTEMS_INLINE_ROUTINE void _Priority_bit_map_Initialize_information(
  Priority_bit_map_Information *the_priority_map,
  Priority_Control      new_priority
)
{
  Priority_bit_map_Control major;
  Priority_bit_map_Control minor;
  Priority_bit_map_Control mask;

  major = _Priority_Major( new_priority );
  minor = _Priority_Minor( new_priority );

  the_priority_map->minor =
    &_Priority_Bit_map[ _Priority_Bits_index(major) ];

  mask = _Priority_Mask( major );
  the_priority_map->ready_major = mask;
  /* Add _Priority_Mask_invert to non-generic bitfield then change this code. */
  the_priority_map->block_major = (Priority_bit_map_Control)(~((uint32_t)mask));

  mask = _Priority_Mask( minor );
  the_priority_map->ready_minor = mask;
  /* Add _Priority_Mask_invert to non-generic bitfield then change this code. */
  the_priority_map->block_minor = (Priority_bit_map_Control)(~((uint32_t)mask));
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
