/*  stack.inl
 *
 *  This file contains the macro implementation of the inlined
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
  ( (_size) >= STACK_MINIMUM_SIZE )

/*PAGE
 *
 *  _Stack_Adjust_size
 */

#define _Stack_Adjust_size( _size ) \
  ((_size) + CPU_STACK_ALIGNMENT)

#endif
/* end of include file */
