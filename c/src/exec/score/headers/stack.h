/*  stack.h
 *
 *  This include file contains all information about the thread
 *  Stack Handler.  This Handler provides mechanisms which can be used to
 *  initialize and utilize stacks.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __STACK_h
#define __STACK_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following constant defines the minimum stack size which every
 *  thread must exceed.
 */

#define STACK_MINIMUM_SIZE  CPU_STACK_MINIMUM_SIZE

/*
 *  The following defines the control block used to manage each stack.
 */

typedef struct {
  unsigned32  size;        /* stack size */
  void       *area;        /* low memory addr of stack */
}   Stack_Control;

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/stack.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
