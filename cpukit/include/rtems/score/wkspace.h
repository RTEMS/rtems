/**
 * @file
 *
 * @ingroup RTEMSScoreWorkspace
 *
 * @brief Information Related to the RAM Workspace
 *
 * This include file contains information related to the
 * RAM Workspace.  This Handler provides mechanisms which can be used to
 * define, initialize and manipulate the workspace.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_WKSPACE_H
#define _RTEMS_SCORE_WKSPACE_H

#include <rtems/score/heap.h>
#include <rtems/score/interr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreWorkspace Workspace Handler
 *
 * @ingroup RTEMSScore
 *
 * This handler encapsulates functionality related to the management of
 * the RTEMS Executive Workspace.
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
 * @brief Initilizes the workspace handler.
 *
 * This routine performs the initialization necessary for this handler.
 *
 * @param areas The heap area for the new workspace.
 * @param area_count The number of areas for the allocation.
 * @param extend The extension handler for the new workspace.
 */
void _Workspace_Handler_initialization(
  Heap_Area *areas,
  size_t area_count,
  Heap_Initialization_or_extend_handler extend
);

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
 * @brief Allocates aligned memory from workspace.
 *
 * @param size The size of the requested memory.
 * @param alignment The alignment of the requested memory.
 *
 * @retval other The memory area begin.
 * @retval NULL Not enough resources.
 */
void *_Workspace_Allocate_aligned( size_t size, size_t alignment );

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
 * @brief Allocates workspace or fails with fatal error.
 *
 * This routine returns the address of a block of memory of @a size
 * bytes.  If a block of the appropriate size cannot be allocated
 * from the workspace, then the internal error handler is invoked.
 *
 * @param size is the desired number of bytes to allocate
 *
 * @return The starting address of the allocated memory.
 */
void *_Workspace_Allocate_or_fatal_error(
  size_t  size
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
