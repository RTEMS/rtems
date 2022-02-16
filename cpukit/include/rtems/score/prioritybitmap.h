/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScorePriorityBitmap
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScorePriorityBitmap which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_PRIORITYBITMAP_H
#define _RTEMS_SCORE_PRIORITYBITMAP_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScorePriorityBitmap Priority Bitmap
 *
 * @brief This group contains the implementation to support priority bitmaps.
 *
 * @ingroup RTEMSScore
 *
 * @{
 */

typedef uint16_t Priority_bit_map_Word;

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

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
