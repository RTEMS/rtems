/*  rtems/posix/cond.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX condition variables.
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
 
#ifndef __RTEMS_POSIX_CONDITION_VARIABLES_inl
#define __RTEMS_POSIX_CONDITION_VARIABLES_inl
 
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
 *  _POSIX_Condition_variables_Get
 */
 
RTEMS_INLINE_ROUTINE POSIX_Condition_variables_Control *_POSIX_Condition_variables_Get (
  Objects_Id        *id,
  Objects_Locations *location
)
{
/* XXX really should validate pointer */
  return (POSIX_Condition_variables_Control *)
    _Objects_Get( &_POSIX_Condition_variables_Information, *id, location );
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

