/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreWorkspace
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreWorkspace which are only used by the implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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

#ifndef _RTEMS_SCORE_WKSPACE_H
#define _RTEMS_SCORE_WKSPACE_H

#include <rtems/score/heap.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreWorkspace
 *
 * @{
 */

/**
 *  @brief Executive workspace control.
 *
 *  This is the heap control structure used to manage the RTEMS Executive
 *  Workspace.
 */
extern Heap_Control _Workspace_Area;

/**
 * @brief Initializes the workspace handler.
 *
 * This routine performs the initialization necessary for this handler.
 */
void _Workspace_Handler_initialization( void );

/**
 * @brief Allocates a memory block of the specified size from the workspace.
 *
 * @param size The size of the memory block.
 *
 * @retval pointer The pointer to the memory block.  The pointer is at least
 *   aligned by CPU_HEAP_ALIGNMENT.
 * @retval NULL No memory block with the requested size is available in the
 *   workspace.
 */
void *_Workspace_Allocate( size_t size );

/**
 * @brief Frees memory to the workspace.
 *
 * This function frees the specified block of memory.
 *
 * @param block The memory to free.
 *
 * @note If @a block is equal to NULL, then the request is ignored.
 *        This allows the caller to not worry about whether or not
 *        a pointer is NULL.
 */
void _Workspace_Free(
  void *block
);

/**
 * @brief Duplicates string with memory from the workspace.
 *
 * @param string The pointer to a zero terminated string.
 * @param len The length of the string (equal to strlen(string)).
 *
 * @retval other Duplicated string.
 * @retval NULL Not enough memory.
 */
char *_Workspace_String_duplicate(
  const char *string,
  size_t len
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
