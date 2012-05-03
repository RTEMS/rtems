/**
 *  @file  rtems/score/wkspace.h
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_WKSPACE_H
#define _RTEMS_SCORE_WKSPACE_H

/**
 *  @defgroup ScoreWorkspace Workspace Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to the management of
 *  the RTEMS Executive Workspace.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/heap.h>
#include <rtems/score/interr.h>

/**
 * @brief Executive Workspace Control
 *
 *  The is the heap control structure that used to manage the
 *  RTEMS Executive Workspace.
 */
SCORE_EXTERN Heap_Control _Workspace_Area;  /* executive heap header */

/**
 * @brief Workspace Handler Initialization
 *
 *  This routine performs the initialization necessary for this handler.
 */
void _Workspace_Handler_initialization(void);

/**
 * @brief Allocate Memory from Workspace
 *
 *  This routine returns the address of a block of memory of size
 *  bytes.  If a block of the appropriate size cannot be allocated
 *  from the workspace, then NULL is returned.
 *
 *  @param size is the requested size
 *
 *  @return a pointer to the requested memory or NULL.
 */
void *_Workspace_Allocate(
  size_t   size
);

/**
 * @brief Free Memory to the Workspace
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
 * @brief Workspace Allocate or Fail with Fatal Error
 *
 *  This routine returns the address of a block of memory of @a size
 *  bytes.  If a block of the appropriate size cannot be allocated
 *  from the workspace, then the internal error handler is invoked.
 *
 *  @param[in] size is the desired number of bytes to allocate
 *  @return If successful, the starting address of the allocated memory
 */
void *_Workspace_Allocate_or_fatal_error(
  size_t  size
);

/**
 * @brief Duplicates the @a string with memory from the Workspace.
 *
 * @param[in] string Pointer to zero terminated string.
 * @param[in] len Length of the string (equal to strlen(string)).
 *
 * @return NULL Not enough memory.
 * @return other Duplicated string.
 */
char *_Workspace_String_duplicate(
  const char *string,
  size_t len
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/wkspace.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
