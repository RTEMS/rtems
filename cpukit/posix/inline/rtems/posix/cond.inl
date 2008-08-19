/**
 * @file rtems/posix/cond.inl
 */

/*  rtems/posix/cond.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX condition variables.
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 
#ifndef _RTEMS_POSIX_COND_H
# error "Never use <rtems/posix/cond.inl> directly; include <rtems/posix/cond.h> instead."
#endif

#ifndef _RTEMS_POSIX_COND_INL
#define _RTEMS_POSIX_COND_INL
 
#include <pthread.h>

/*PAGE
 *
 *  _POSIX_Condition_variables_Allocate
 */
 
RTEMS_INLINE_ROUTINE POSIX_Condition_variables_Control
  *_POSIX_Condition_variables_Allocate( void )
{
  return (POSIX_Condition_variables_Control *) 
    _Objects_Allocate( &_POSIX_Condition_variables_Information );
}
 
/*PAGE
 *
 *  _POSIX_Condition_variables_Free
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Condition_variables_Free (
  POSIX_Condition_variables_Control *the_condition_variable
)
{
  _Objects_Free(
    &_POSIX_Condition_variables_Information,
    &the_condition_variable->Object
  );
}
 
/*PAGE
 *
 *  _POSIX_Condition_variables_Is_null
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Condition_variables_Is_null (
  POSIX_Condition_variables_Control *the_condition_variable
)
{
  return !the_condition_variable;
}

#endif
/*  end of include file */

