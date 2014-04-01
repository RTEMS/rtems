/**
 *  @file  rtems/score/prioritybitmap.h
 *
 *  @brief Manipulation Routines for the Bitmap Priority Queue Implementation
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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_PRIORITYBITMAP_H
#define _RTEMS_SCORE_PRIORITYBITMAP_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScorePriorityBitmap Bitmap Priority Thread Routines
 *
 *  @ingroup Score
 */
/**@{*/

/*
 *  The definition of the Priority_bit_map_Word type is CPU dependent.
 *
 */

typedef struct {
  /**
   * @brief Each sixteen bit entry in this word is associated with one of the
   * sixteen entries in the bit map.
   */
  Priority_bit_map_Word major_bit_map;

  /**
   * @brief Each bit in the bit map indicates whether or not there are threads
   * ready at a particular priority.
   *
   * The mapping of individual priority levels to particular bits is processor
   * dependent as is the value of each bit used to indicate that threads are
   * ready at that priority.
   */
  Priority_bit_map_Word bit_map[ 16 ];
} Priority_bit_map_Control;

/**
 *  The following record defines the information associated with
 *  each thread to manage its interaction with the priority bit maps.
 */
typedef struct {
  /** This is the address of minor bit map slot. */
  Priority_bit_map_Word *minor;
  /** This is the priority bit map ready mask. */
  Priority_bit_map_Word  ready_major;
  /** This is the priority bit map ready mask. */
  Priority_bit_map_Word  ready_minor;
  /** This is the priority bit map block mask. */
  Priority_bit_map_Word  block_major;
  /** This is the priority bit map block mask. */
  Priority_bit_map_Word  block_minor;
} Priority_bit_map_Information;

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
