/*  priority.inl
 *
 *  This file contains the static inline implementation of all inlined
 *  routines in the Priority Handler.
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

#ifndef __PRIORITY_inl
#define __PRIORITY_inl

#include <rtems/score/bitfield.h>

/*PAGE
 *
 *  _Priority_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

RTEMS_INLINE_ROUTINE void _Priority_Handler_initialization( void )
{
  unsigned32 index;

  _Priority_Major_bit_map = 0;
  for ( index=0 ; index <16 ; index++ )
     _Priority_Bit_map[ index ] = 0;
}

/*PAGE
 *
 *  _Priority_Is_valid
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_priority if valid for a
 *  user task, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Priority_Is_valid (
  Priority_Control the_priority
)
{
  /* 
   *  Since PRIORITY_MINIMUM is 0 and priorities are stored unsigned,
   *  then checking for less than 0 is unnecessary.
   */

  return ( the_priority <= PRIORITY_MAXIMUM );
}

/*PAGE
 *
 *  _Priority_Major
 *
 *  DESCRIPTION:
 *
 *  This function returns the major portion of the_priority.
 */

RTEMS_INLINE_ROUTINE unsigned32 _Priority_Major (
  Priority_Control the_priority
)
{
  return ( the_priority / 16 );
}

/*PAGE
 *
 *  _Priority_Minor
 *
 *  DESCRIPTION:
 *
 *  This function returns the minor portion of the_priority.
 */

RTEMS_INLINE_ROUTINE unsigned32 _Priority_Minor (
  Priority_Control the_priority
)
{
  return ( the_priority % 16 );
}

#if ( CPU_USE_GENERIC_BITFIELD_CODE == TRUE )
 
/*PAGE
 *
 *  _Priority_Mask
 *
 *  DESCRIPTION:
 *
 *  This function returns the mask associated with the major or minor
 *  number passed to it.
 */
 
RTEMS_INLINE_ROUTINE unsigned32 _Priority_Mask (
  unsigned32 bit_number
)
{
  return (0x8000 >> bit_number);
}
 
 
/*PAGE
 *
 *  _Priority_Bits_index
 *
 *  DESCRIPTION:
 *
 *  This function translates the bit numbers returned by the bit scan
 *  of a priority bit field into something suitable for use as
 *  a major or minor component of a priority.
 */
 
RTEMS_INLINE_ROUTINE unsigned32 _Priority_Bits_index (
  unsigned32 bit_number
)
{
  return bit_number;
}

#endif

/*PAGE
 *
 *  _Priority_Add_to_bit_map
 *
 *  DESCRIPTION:
 *
 *  This routine uses the_priority_map to update the priority
 *  bit maps to indicate that a thread has been readied.
 */

RTEMS_INLINE_ROUTINE void _Priority_Add_to_bit_map (
  Priority_Information *the_priority_map
)
{
  *the_priority_map->minor |= the_priority_map->ready_minor;
  _Priority_Major_bit_map  |= the_priority_map->ready_major;
}

/*PAGE
 *
 *  _Priority_Remove_from_bit_map
 *
 *  DESCRIPTION:
 *
 *  This routine uses the_priority_map to update the priority
 *  bit maps to indicate that a thread has been removed from the
 *  ready state.
 */

RTEMS_INLINE_ROUTINE void _Priority_Remove_from_bit_map (
  Priority_Information *the_priority_map
)
{
  *the_priority_map->minor &= the_priority_map->block_minor;
  if ( *the_priority_map->minor == 0 )
    _Priority_Major_bit_map &= the_priority_map->block_major;
}

/*PAGE
 *
 *  _Priority_Get_highest
 *
 *  DESCRIPTION:
 *
 *  This function returns the priority of the highest priority
 *  ready thread.
 */

RTEMS_INLINE_ROUTINE Priority_Control _Priority_Get_highest( void )
{
  Priority_Bit_map_control minor;
  Priority_Bit_map_control major;

  _Bitfield_Find_first_bit( _Priority_Major_bit_map, major );
  _Bitfield_Find_first_bit( _Priority_Bit_map[major], minor );

  return (_Priority_Bits_index( major ) << 4) +
          _Priority_Bits_index( minor );
}

/*PAGE
 *
 *  _Priority_Initialize_information
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the_priority_map so that it
 *  contains the information necessary to manage a thread
 *  at new_priority.
 */

RTEMS_INLINE_ROUTINE void _Priority_Initialize_information(
  Priority_Information *the_priority_map,
  Priority_Control      new_priority
)
{
  Priority_Bit_map_control major;
  Priority_Bit_map_control minor;
  Priority_Bit_map_control mask;

  major = _Priority_Major( new_priority );
  minor = _Priority_Minor( new_priority );

  the_priority_map->minor =
    &_Priority_Bit_map[ _Priority_Bits_index(major) ];

  mask = _Priority_Mask( major );
  the_priority_map->ready_major = mask;
  the_priority_map->block_major = ~mask;

  mask = _Priority_Mask( minor );
  the_priority_map->ready_minor = mask;
  the_priority_map->block_minor = ~mask;
}

/*PAGE
 *
 *  _Priority_Is_group_empty
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the priority GROUP is empty, and
 *  FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Priority_Is_group_empty (
  Priority_Control      the_priority
)
{
  return the_priority == 0;
}

#endif
/* end of include file */
