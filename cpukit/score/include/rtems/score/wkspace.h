/*  wkspace.h
 *
 *  This include file contains information related to the RTEMS
 *  RAM Workspace.  This Handler provides mechanisms which can be used to
 *  define, initialize and manipulate the RTEMS workspace.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_WORKSPACE_h
#define __RTEMS_WORKSPACE_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/heap.h>
#include <rtems/fatal.h>
#include <rtems/status.h>

/*
 *  The following is used to manage the RTEMS Workspace.
 *
 */

EXTERN Heap_Control _Workspace_Area;  /* executive heap header */

/*
 *  _Workspace_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

STATIC INLINE void _Workspace_Handler_initialization(
  void       *starting_address,
  unsigned32  size
);

/*
 *  _Workspace_Allocate
 *
 *  DESCRIPTION:
 *
 *  This routine returns the address of a block of memory of size
 *  bytes.  If a block of the appropriate size cannot be allocated
 *  from the workspace, then NULL is returned.
 */

STATIC INLINE void *_Workspace_Allocate(
  unsigned32 size
);

/*
 *  _Workspace_Allocate_or_fatal_error
 *
 *  DESCRIPTION:
 *
 *  This routine returns the address of a block of memory of size
 *  bytes.  If a block of the appropriate size cannot be allocated
 *  from the workspace, then the rtems_fatal_error_occurred directive
 *  is invoked.
 */

void *_Workspace_Allocate_or_fatal_error(
  unsigned32   size
);

/*
 *  _Workspace_Free
 *
 *  DESCRIPTION:
 *
 *  This function frees the specified block of memory.  If the block
 *  belongs to the Workspace and can be successfully freed, then
 *  TRUE is returned.  Otherwise FALSE is returned.
 */

STATIC INLINE boolean _Workspace_Free(
  void *block
);

#include <rtems/wkspace.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
