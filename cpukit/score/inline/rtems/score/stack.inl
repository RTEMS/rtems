/*  stack.inl
 *
 *  This file contains the static inline implementation of the inlined
 *  routines from the Stack Handler.
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

#ifndef __STACK_inl
#define __STACK_inl

/*PAGE
 *
 *  _Stack_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the_stack record to indicate that
 *  size bytes of memory starting at starting_address have been
 *  reserved for a stack.
 */

RTEMS_INLINE_ROUTINE void _Stack_Initialize (
  Stack_Control *the_stack,
  void          *starting_address,
  unsigned32     size
)
{
  the_stack->area = starting_address;
  the_stack->size = size;
}

/*PAGE
 *
 *  _Stack_Is_enough
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if size bytes is enough memory for
 *  a valid stack area on this processor, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Stack_Is_enough (
  unsigned32 size
)
{
  return ( size >= STACK_MINIMUM_SIZE );
}

/*PAGE
 *
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

RTEMS_INLINE_ROUTINE unsigned32 _Stack_Adjust_size (
  unsigned32 size
)
{
  return size + CPU_STACK_ALIGNMENT;
}

#endif
/* end of include file */
