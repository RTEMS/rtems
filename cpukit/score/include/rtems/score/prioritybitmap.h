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
 *  http://www.rtems.com/license/LICENSE.
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

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
