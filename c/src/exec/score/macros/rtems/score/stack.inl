/*  stack.inl
 *
 *  This file contains the macro implementation of the inlined
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

#define _Stack_Initialize( _the_stack, _starting_address, _size ) \
  { \
    (_the_stack)->area = (_starting_address); \
    (_the_stack)->size = (_size);  \
  }

/*PAGE
 *
 *  _Stack_Is_enough
 *
 */

#define _Stack_Is_enough( _size ) \
  ( (_size) >= RTEMS_MINIMUM_STACK_SIZE )

/*PAGE
 *
 *  _Stack_Adjust_size
 */

#define _Stack_Adjust_size( _size ) \
  ((_size) + CPU_STACK_ALIGNMENT)

#endif
/* end of include file */
