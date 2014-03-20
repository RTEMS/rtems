/**
 *  @file rtems/score/wkspace.h
 *
 *  @brief Information Related to the RAM Workspace
 *
 *  This include file contains information related to the
 *  RAM Workspace.  This Handler provides mechanisms which can be used to
 *  define, initialize and manipulate the workspace.
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
 *  @defgroup ScoreWorkspace Workspace Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to the management of
 *  the RTEMS Executive Workspace.
 */
/**@{*/

/**
 *  @brief Executive workspace control.
 *
 *  This is the heap control structure used to manage the RTEMS Executive
 *  Workspace.
 */
SCORE_EXTERN Heap_Control _Workspace_Area;

/**
 * @brief Initilize workspace handler.
 *
 *  This routine performs the initialization necessary for this handler.
 */
void _Workspace_Handler_initialization(
  Heap_Area *areas,
  size_t area_count,
  Heap_Initialization_or_extend_handler extend
);

/**
 * @brief Allocate memory from workspace.
 *
 *  This routine returns the address of a block of memory of size
 *  bytes.  If a block of the appropriate size cannot be allocated
 *  from the workspace, then NULL is returned.
 *
 *  @param size is the requested size
 *
 *  @retval a pointer to the requested memory or NULL.
 */
void *_Workspace_Allocate(
  size_t   size
);

/**
 * @brief Allocate aligned memory from workspace.
 *
 * @param[in] size The size of the requested memory.
 * @param[in] alignment The alignment of the requested memory.
 *
 * @retval NULL Not enough resources.
 * @retval other The memory area begin.
 */
void *_Workspace_Allocate_aligned( size_t size, size_t alignment );

/**
 * @brief Free memory to the workspace.
 *
 *  This function frees the specified block of memory.  If the block
 *  belongs to the Workspace and can be successfully freed, then
 *  true is returned.  Otherwise false is returned.
 *
 *  @param block is the memory to free
 *
 *  @note If @a block is equal to NULL, then the request is ignored.
 *        This allows the caller to not worry about whether or not
 *        a pointer is NULL.
 */

void _Workspace_Free(
  void *block
);

/**
 * @brief Workspace allocate or fail with fatal error.
 *
 *  This routine returns the address of a block of memory of @a size
 *  bytes.  If a block of the appropriate size cannot be allocated
 *  from the workspace, then the internal error handler is invoked.
 *
 *  @param[in] size is the desired number of bytes to allocate
 *  @retval If successful, the starting address of the allocated memory
 */
void *_Workspace_Allocate_or_fatal_error(
  size_t  size
);

/**
 * @brief Duplicates string with memory from the workspace.
 *
 * @param[in] string is the pointer to a zero terminated string.
 * @param[in] len is the length of the string (equal to strlen(string)).
 *
 * @retval NULL Not enough memory.
 * @retval other Duplicated string.
 */
char *_Workspace_String_duplicate(
  const char *string,
  size_t len
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
