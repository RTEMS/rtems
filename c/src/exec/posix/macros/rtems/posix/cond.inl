/*  rtems/posix/cond.inl
 *
 *  This include file contains the macro implementation of the private 
 *  inlined routines for POSIX condition variables.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
 
#ifndef __RTEMS_POSIX_CONDITION_VARIABLES_inl
#define __RTEMS_POSIX_CONDITION_VARIABLES_inl
 
/*PAGE
 *
 *  _POSIX_Condition_variables_Allocate
 */
 
#define _POSIX_Condition_variables_Allocate() \
  (POSIX_Condition_variables_Control *) \
    _Objects_Allocate( &_POSIX_Condition_variables_Information )
 
/*PAGE
 *
 *  _POSIX_Condition_variables_Free
 */
 
#define _POSIX_Condition_variables_Free( _the_condition_variable ) \
  _Objects_Free( &_POSIX_Condition_variables_Information, \
    &(_the_condition_variable)->Object)
 
/*PAGE
 *
 *  _POSIX_Condition_variables_Get
 */
 
/* XXX how to write this as a macro */
static POSIX_Condition_variables_Control *_POSIX_Condition_variables_Get (
  Objects_Id        *id,
  Objects_Locations *location
)
{
/* XXX should support COND_INITIALIZER */
  if ( id )
    return (POSIX_Condition_variables_Control *)
      _Objects_Get( &_POSIX_Condition_variables_Information, *id, location );

  *location = OBJECTS_ERROR;
  return NULL;
}
 
/*PAGE
 *
 *  _POSIX_Condition_variables_Is_null
 */
 
#define _POSIX_Condition_variables_Is_null( _the_condition_variable ) \
  (!(_the_condition_variable))

#endif
/*  end of include file */
