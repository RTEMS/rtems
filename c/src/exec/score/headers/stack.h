/*  stack.h
 *
 *  This include file contains all information about the thread
 *  Stack Handler.  This Handler provides mechanisms which can be used to
 *  initialize and utilize stacks.
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

/*
 *  _Stack_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the_stack record to indicate that
 *  size bytes of memory starting at starting_address have been
 *  reserved for a stack.
 */

STATIC INLINE void _Stack_Initialize (
  Stack_Control    *the_stack,
  void             *starting_address,
  unsigned32        size
);

/*
 *  _Stack_Is_enough
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if size bytes is enough memory for
 *  a valid stack area on this processor, and FALSE otherwise.
 */

STATIC INLINE boolean _Stack_Is_enough (
  unsigned32 size
);

/*
 *  _Stack_Adjust_size
 *
 *  DESCRIPTION:
 *
 *  This function increases the stack size to insure that the thread
 *  has the desired amount of stack space after the initial stack
 *  pointer is determined based on alignment restrictions.
 *
 *  NOTE:
 *
 *  The amount of adjustment for alignment is CPU dependent.
 */

STATIC INLINE unsigned32 _Stack_Adjust_size (
  unsigned32 size
);

#include <rtems/core/stack.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
