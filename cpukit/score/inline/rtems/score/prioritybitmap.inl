/** 
 *  @file  rtems/score/prioritybitmap.inl
 *
 *  This file contains the static inline implementation of all inlined
 *  routines in the Priority Handler bit map implementation
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_PRIORITYBITMAP_H
# error "Never use <rtems/score/prioritybitmap.inl> directly; include <rtems/score/prioritybitmap.h> instead."
#endif

#ifndef _RTEMS_SCORE_PRIORITYBITMAP_INL
#define _RTEMS_SCORE_PRIORITYBITMAP_INL

/**
 *  @addtogroup ScorePriority 
 *  @{
 */

#include <rtems/score/bitfield.h>

/**
 *  This function returns the major portion of the_priority.
 */

RTEMS_INLINE_ROUTINE Priority_bit_map_Control   _Priority_Major (
  Priority_Control the_priority
)
{
  return (Priority_bit_map_Control)( the_priority / 16 );
}

/**
 *  This function returns the minor portion of the_priority.
 */

RTEMS_INLINE_ROUTINE Priority_bit_map_Control   _Priority_Minor (
  Priority_Control the_priority
)
{
  return (Priority_bit_map_Control)( the_priority % 16 );
}

#if ( CPU_USE_GENERIC_BITFIELD_CODE == TRUE )
 
/**
 *  This function returns the mask associated with the major or minor
 *  number passed to it.
 */
 
RTEMS_INLINE_ROUTINE Priority_bit_map_Control   _Priority_Mask (
  uint32_t   bit_number
)
{
  return (Priority_bit_map_Control)(0x8000u >> bit_number);
}
 
/**
 *  This function returns the mask bit inverted.
 */
 
RTEMS_INLINE_ROUTINE Priority_bit_map_Control   _Priority_Mask_invert (
  uint32_t   mask
)
{
  return (Priority_bit_map_Control)(~mask);
}

 
/**
 *  This function translates the bit numbers returned by the bit scan
 *  of a priority bit field into something suitable for use as
 *  a major or minor component of a priority.
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
 *  This routine performs the initialization necessary for this handler.
 */

RTEMS_INLINE_ROUTINE void _Priority_bit_map_Handler_initialization( void )
{
  int index;

  _Priority_Major_bit_map = 0;
  for ( index=0 ; index <16 ; index++ )
     _Priority_Bit_map[ index ] = 0;
}

/**
 *  This routine uses the_priority_map to update the priority
 *  bit maps to indicate that a thread has been readied.
 */

RTEMS_INLINE_ROUTINE void _Priority_bit_map_Add (
  Priority_bit_map_Information *the_priority_map
)
{
  *the_priority_map->minor |= the_priority_map->ready_minor;
  _Priority_Major_bit_map  |= the_priority_map->ready_major;
}

/**
 *  This routine uses the_priority_map to update the priority
 *  bit maps to indicate that a thread has been removed from the
 *  ready state.
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
 *  This function returns the priority of the highest priority
 *  ready thread.
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

/**
 *  This routine initializes the_priority_map so that it
 *  contains the information necessary to manage a thread
 *  at new_priority.
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

/**@}*/

#endif
/* end of include file */
