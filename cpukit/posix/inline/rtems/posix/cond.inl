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
 
#ifndef __RTEMS_POSIX_CONDITION_VARIABLES_inl
#define __RTEMS_POSIX_CONDITION_VARIABLES_inl
 
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
 *  _POSIX_Condition_variables_Get
 */

RTEMS_INLINE_ROUTINE POSIX_Condition_variables_Control 
*_POSIX_Condition_variables_Get (
  Objects_Id        *id,
  Objects_Locations *location
)
{
  int status;

  if ( !id ) {
    *location = OBJECTS_ERROR;
    return (POSIX_Condition_variables_Control *) 0;
  }

  if ( *id == PTHREAD_COND_INITIALIZER ) {
    /*
     *  Do an "auto-create" here.
     */

    status = pthread_cond_init( id, 0 );
    if ( status ) {
      *location = OBJECTS_ERROR;
      return (POSIX_Condition_variables_Control *) 0;
    }
  }

  /*
   *  Now call Objects_Get()
   */

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

