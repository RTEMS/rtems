/*  priority.h
 *
 *  This include file contains all thread priority manipulation routines.
 *  This Handler provides mechanisms which can be used to
 *  initialize and manipulate thread priorities.
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

#ifndef __PRIORITY_h
#define __PRIORITY_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following type defines the control block used to manage
 *  thread priorities.
 *
 *  NOTE: Priority 0 is reserved for internal threads only.
 */

typedef unsigned32 Priority_Control;

#define PRIORITY_MINIMUM      0         /* highest thread priority */
#define PRIORITY_MAXIMUM      255       /* lowest thread priority */

/*
 *  The following record defines the information associated with
 *  each thread to manage its interaction with the priority bit maps.
 */

typedef struct {
  Priority_Bit_map_control *minor;        /* addr of minor bit map slot */
  Priority_Bit_map_control  ready_major;  /* priority bit map ready mask */
  Priority_Bit_map_control  ready_minor;  /* priority bit map ready mask */
  Priority_Bit_map_control  block_major;  /* priority bit map block mask */
  Priority_Bit_map_control  block_minor;  /* priority bit map block mask */
}   Priority_Information;

/*
 *  The following data items are the priority bit map.
 *  Each of the sixteen bits used in the _Priority_Major_bit_map is
 *  associated with one of the sixteen entries in the _Priority_Bit_map.
 *  Each bit in the _Priority_Bit_map indicates whether or not there are
 *  threads ready at a particular priority.  The mapping of
 *  individual priority levels to particular bits is processor
 *  dependent as is the value of each bit used to indicate that
 *  threads are ready at that priority.
 */

SCORE_EXTERN volatile Priority_Bit_map_control _Priority_Major_bit_map;
SCORE_EXTERN Priority_Bit_map_control 
               _Priority_Bit_map[16] CPU_STRUCTURE_ALIGNMENT;

/*
 *  The definition of the Priority_Bit_map_control type is CPU dependent.
 *
 */

/*
 *  Priority Bitfield Manipulation Routines
 *
 *  NOTE:
 *
 *  These may simply be pass throughs to CPU dependent routines.
 */
 
#if ( CPU_USE_GENERIC_BITFIELD_CODE == FALSE )

#define _Priority_Mask( _bit_number ) \
  _CPU_Priority_Mask( _bit_number )
 
#define _Priority_Bits_index( _priority ) \
  _CPU_Priority_bits_index( _priority )

#endif
 
#ifndef __RTEMS_APPLICATION__
#include <rtems/score/priority.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
