/**
 *  @file  rtems/score/prioritybitmap.h
 *
 *  This include file contains all thread priority manipulation routines for
 *  the bit map priority queue implementation.
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
#define _RTEMS_SCORE_PRIORITYBITMAP_H

/**
 *  @addtogroup ScorePriority
 *
 */
/**@{*/

/*
 * Processor specific information.
 */
#include <rtems/score/cpu.h>


#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/priority.h>


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

/*
 *  The definition of the Priority_bit_map_Control type is CPU dependent.
 *
 */

/**
 *  The following record defines the information associated with
 *  each thread to manage its interaction with the priority bit maps.
 */
typedef struct {
  /** This is the address of minor bit map slot. */
  Priority_bit_map_Control *minor;
  /** This is the priority bit map ready mask. */
  Priority_bit_map_Control  ready_major;
  /** This is the priority bit map ready mask. */
  Priority_bit_map_Control  ready_minor;
  /** This is the priority bit map block mask. */
  Priority_bit_map_Control  block_major;
  /** This is the priority bit map block mask. */
  Priority_bit_map_Control  block_minor;
} Priority_bit_map_Information;


#if ( CPU_USE_GENERIC_BITFIELD_CODE == FALSE )
/**
 *  This method returns the priority bit mask for the specified major
 *  or minor bit number.
 *
 *  @param[in] _bit_number is the bit number for which we need a mask
 *
 *  @return the priority bit mask
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
 *  @return This method returns the array index into the priority bit map.
 *
 *  @note This may simply be a pass through to a CPU dependent implementation.
 */
#define _Priority_Bits_index( _priority ) \
  _CPU_Priority_bits_index( _priority )
#endif

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/prioritybitmap.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
