/*  priority.inl
 *
 *  This file contains the macro implementation of all inlined routines
 *  in the Priority Handler.
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

#include <rtems/core/bitfield.h>

/*PAGE
 *
 *  _Priority_Handler_initialization
 *
 */

#define _Priority_Handler_initialization() \
  { \
    unsigned32 index; \
    \
    _Priority_Major_bit_map = 0; \
    for ( index=0 ; index <16 ; index++ ) \
       _Priority_Bit_map[ index ] = 0; \
  }

/*PAGE
 *
 *  _Priority_Is_valid
 *
 */

  /* 
   *  Since PRIORITY_MINIMUM is 0 and priorities are stored unsigned,
   *  then checking for less than 0 is unnecessary.
   */

#define _Priority_Is_valid( _the_priority ) \
  (  (_the_priority) <= PRIORITY_MAXIMUM )

/*PAGE
 *
 *  _Priority_Major
 *
 */

#define _Priority_Major( _the_priority ) ( (_the_priority) / 16 )

/*PAGE
 *
 *  _Priority_Minor
 *
 */

#define _Priority_Minor( _the_priority ) ( (_the_priority) % 16 )

/*PAGE
 *
 *  _Priority_Add_to_bit_map
 *
 */

#define _Priority_Add_to_bit_map( _the_priority_map ) \
   { \
     *(_the_priority_map)->minor |= (_the_priority_map)->ready_minor; \
     _Priority_Major_bit_map     |= (_the_priority_map)->ready_major; \
   }

/*PAGE
 *
 *  _Priority_Remove_from_bit_map
 *
 */

#define _Priority_Remove_from_bit_map( _the_priority_map ) \
   { \
     *(_the_priority_map)->minor &= (_the_priority_map)->block_minor; \
     if ( *(_the_priority_map)->minor == 0 ) \
       _Priority_Major_bit_map &= (_the_priority_map)->block_major; \
   }

/*PAGE
 *
 *  _Priority_Get_highest
 *
 */

#define _Priority_Get_highest( _high_priority ) \
  { \
    Priority_Bit_map_control minor; \
    Priority_Bit_map_control major; \
    \
    _Bitfield_Find_first_bit( _Priority_Major_bit_map, major ); \
    _Bitfield_Find_first_bit( _Priority_Bit_map[major], minor ); \
    \
    (_high_priority) = (_CPU_Priority_Bits_index( major ) * 16) +  \
                       _CPU_Priority_Bits_index( minor ); \
  }

/*PAGE
 *
 *  _Priority_Initialize_information
 *
 */

#define _Priority_Initialize_information( \
     _the_priority_map, _new_priority ) \
  { \
    Priority_Bit_map_control _major; \
    Priority_Bit_map_control _minor; \
    Priority_Bit_map_control _mask;  \
    \
    _major = _Priority_Major( (_new_priority) ); \
    _minor = _Priority_Minor( (_new_priority) ); \
    \
    (_the_priority_map)->minor =  \
      &_Priority_Bit_map[ _CPU_Priority_Bits_index(_major) ]; \
    \
    _mask = _CPU_Priority_Mask( _major ); \
    (_the_priority_map)->ready_major = _mask; \
    (_the_priority_map)->block_major = ~_mask; \
    \
    _mask = _CPU_Priority_Mask( _minor ); \
    (_the_priority_map)->ready_minor = _mask; \
    (_the_priority_map)->block_minor = ~_mask; \
  }

/*PAGE
 *
 *  _Priority_Is_group_empty
 *
 */

#define _Priority_Is_group_empty ( _the_priority ) \
  ( (_the_priority) == 0 )

#endif
/* end of include file */
