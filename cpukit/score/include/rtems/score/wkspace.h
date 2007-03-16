/** 
 *  @file  rtems/score/wkspace.h
 *
 *  This include file contains information related to the
 *  RAM Workspace.  This Handler provides mechanisms which can be used to
 *  define, initialize and manipulate the workspace.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_WKSPACE_H
#define _RTEMS_SCORE_WKSPACE_H

/**
 *  @defgroup ScoreWorkspace Workspace Handler
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

/** @brief Executive Workspace Control
 *
 *  The is the heap control structure that used to manage the 
 *  RTEMS Executive Workspace.
 */
SCORE_EXTERN Heap_Control _Workspace_Area;  /* executive heap header */

/** @brief Workspace Handler Initialization
 *
 *  This routine performs the initialization necessary for this handler.
 *
 *  @param[in] starting_address is the base address of the RTEMS Executive
 *         Workspace
 *  @param[in] size is the number of bytes in the RTEMS Executive Workspace
 */
void _Workspace_Handler_initialization(
  void       *starting_address,
  size_t      size
);

/** @brief Workspace Allocate or Fail with Fatal Error
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

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/wkspace.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
