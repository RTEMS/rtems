/*  wkspace.h
 *
 *  This include file contains information related to the 
 *  RAM Workspace.  This Handler provides mechanisms which can be used to
 *  define, initialize and manipulate the workspace.
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

#ifndef __WORKSPACE_h
#define __WORKSPACE_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/heap.h>
#include <rtems/score/interr.h>

/*
 *  The following is used to manage the Workspace.
 *
 */

SCORE_EXTERN Heap_Control _Workspace_Area;  /* executive heap header */

/*
 *  _Workspace_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */
 
void _Workspace_Handler_initialization(
  void       *starting_address,
  unsigned32  size
);

/*
 *  _Workspace_Allocate_or_fatal_error
 *
 *  DESCRIPTION:
 *
 *  This routine returns the address of a block of memory of size
 *  bytes.  If a block of the appropriate size cannot be allocated
 *  from the workspace, then the internal error handler is invoked.
 */

void *_Workspace_Allocate_or_fatal_error(
  unsigned32   size
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/wkspace.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
