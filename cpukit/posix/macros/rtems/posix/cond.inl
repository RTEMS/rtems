/*  rtems/posix/cond.inl
 *
 *  This include file contains the macro implementation of the private 
 *  inlined routines for POSIX condition variables.
 *
 *  COPYRIGHT (c) 1989-2002.
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
 
#include <pthread.h>

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
 *
 *  NOTE: The support macro makes it possible for both to use exactly
 *        the same code to check for NULL id pointer and
 *        PTHREAD_COND_INITIALIZER without adding overhead.
 */
 
#define _POSIX_Condition_variables_Free( _the_condition_variable ) \
  _Objects_Free( &_POSIX_Condition_variables_Information, \
    &(_the_condition_variable)->Object)
 
/*PAGE
 *
 *  _POSIX_Condition_variables_Get
 *
 *  NOTE: The support macro makes it possible for both to use exactly
 *        the same code to check for NULL id pointer and
 *        PTHREAD_COND_INITIALIZER without adding overhead.
 */

#define ___POSIX_Condition_variables_Get_support( _id, _location ) \
  do { \
    int _status; \
    \
    if ( !_id ) { \
      *_location = OBJECTS_ERROR; \
      return (POSIX_Condition_variables_Control *) 0; \
    }  \
    \
    if ( *_id == PTHREAD_COND_INITIALIZER ) { \
      /* \
       *  Do an "auto-create" here. \
       */ \
    \
      _status = pthread_cond_init( _id, 0 ); \
      if ( _status ) { \
        *_location = OBJECTS_ERROR;  \
        return (POSIX_Condition_variables_Control *) 0; \
      } \
    } \
  } while (0)
 
 
static POSIX_Condition_variables_Control *_POSIX_Condition_variables_Get (
  Objects_Id        *id,
  Objects_Locations *location
)
{
  ___POSIX_Condition_variables_Get_support( id, location );

  return (POSIX_Condition_variables_Control *)
      _Objects_Get( &_POSIX_Condition_variables_Information, *id, location );
}
 
/*PAGE
 *
 *  _POSIX_Condition_variables_Is_null
 */
 
#define _POSIX_Condition_variables_Is_null( _the_condition_variable ) \
  (!(_the_condition_variable))

#endif
/*  end of include file */
