/*  stack.inl
 *
 *  This file contains the static inline implementation of the inlined
 *  routines from the Stack Handler.
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

#ifndef __STACK_inl
#define __STACK_inl

/*PAGE
 *
 *  _Stack_Initialize
 *
 */

STATIC INLINE void _Stack_Initialize (
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
 */

STATIC INLINE boolean _Stack_Is_enough (
  unsigned32 size
)
{
  return ( size >= STACK_MINIMUM_SIZE );
}

/*PAGE
 *
 *  _Stack_Adjust_size
 *
 */

STATIC INLINE unsigned32 _Stack_Adjust_size (
  unsigned32 size
)
{
  return size + CPU_STACK_ALIGNMENT;
}

#endif
/* end of include file */
