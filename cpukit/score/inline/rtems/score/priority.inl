/*  priority.inl
 *
 *  This file contains the static inline implementation of all inlined
 *  routines in the Priority Handler.
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

#ifndef __PRIORITY_inl
#define __PRIORITY_inl

#include <rtems/bitfield.h>

/*PAGE
 *
 *  _Priority_Handler_initialization
 *
 */

STATIC INLINE void _Priority_Handler_initialization( void )
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
 */

STATIC INLINE boolean _Priority_Is_valid (
  rtems_task_priority the_priority
)
{
  return (  ( the_priority >= RTEMS_MINIMUM_PRIORITY ) &&
            ( the_priority <= RTEMS_MAXIMUM_PRIORITY ) );
}

/*PAGE
 *
 *  _Priority_Major
 *
 */

STATIC INLINE unsigned32 _Priority_Major (
  rtems_task_priority the_priority
)
{
  return ( the_priority / 16 );
}

/*PAGE
 *
 *  _Priority_Minor
 *
 */

STATIC INLINE unsigned32 _Priority_Minor (
  rtems_task_priority the_priority
)
{
  return ( the_priority % 16 );
}

/*PAGE
 *
 *  _Priority_Add_to_bit_map
 *
 */

STATIC INLINE void _Priority_Add_to_bit_map (
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
 */

STATIC INLINE void _Priority_Remove_from_bit_map (
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
 */

STATIC INLINE rtems_task_priority _Priority_Get_highest( void )
{
  Priority_Bit_map_control minor;
  Priority_Bit_map_control major;

  _Bitfield_Find_first_bit( _Priority_Major_bit_map, major );
  _Bitfield_Find_first_bit( _Priority_Bit_map[major], minor );

  return (_CPU_Priority_Bits_index( major ) << 4) +
          _CPU_Priority_Bits_index( minor );
}

/*PAGE
 *
 *  _Priority_Initialize_information
 *
 */

STATIC INLINE void _Priority_Initialize_information(
  Priority_Information *the_priority_map,
  rtems_task_priority      new_priority
)
{
  Priority_Bit_map_control major;
  Priority_Bit_map_control minor;
  Priority_Bit_map_control mask;

  major = _Priority_Major( new_priority );
  minor = _Priority_Minor( new_priority );

  the_priority_map->minor =
    &_Priority_Bit_map[ _CPU_Priority_Bits_index(major) ];

  mask = _CPU_Priority_Mask( major );
  the_priority_map->ready_major = mask;
  the_priority_map->block_major = ~mask;

  mask = _CPU_Priority_Mask( minor );
  the_priority_map->ready_minor = mask;
  the_priority_map->block_minor = ~mask;
}

/*PAGE
 *
 *  _Priority_Is_group_empty
 *
 */

STATIC INLINE boolean _Priority_Is_group_empty (
  rtems_task_priority      the_priority
)
{
  return the_priority == 0;
}

#endif
/* end of include file */
