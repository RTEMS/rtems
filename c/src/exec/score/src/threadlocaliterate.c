/*
 *  Thread Iterator
 *
 *
 *  COPYRIGHT (c) 2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>


/*PAGE
 *
 *  _Thread_Local_iterate
 *
 *  DESCRIPTION:
 *
 *  This function invokes the callback function for each existing thread.
 *  Iteration continues until either all threads have been processed, or,
 *  if break_on_error is TRUE, until an invocation of the callback returns
 *  an integer value other than 0.
 *
 *  Input parameters:
 *    callback:
 *      A pointer to a function with the indicated signature.
 *    arg: 
 *      A pointer to some arbitrary entity. Passed on to the callback.
 *    break_on_error
 *      If TRUE, stop iterating on error.
 *
 *
 *  Output parameters: NONE
 *    But callback may write into space pointed to by arg.
 *
 *  Return value:
 *    0 if successful
 *    Value returned by the callback otherwise.
 */

unsigned32 _Thread_Local_iterate(
  unsigned32 (*callback)(Thread_Control *the_thread, void * arg),
  void        *arg,
  boolean      break_on_error
)
{
  unsigned32 class_index;
  unsigned32 result;
  Objects_Information *information;
  
  if( callback == NULL )
    return 0;
  
  for ( class_index = OBJECTS_CLASSES_FIRST ;
        class_index <= OBJECTS_CLASSES_LAST ;
        class_index++ ) {
    information = _Objects_Information_table[ class_index ];
    if ( information && information->is_thread ) {
      result = _Objects_Local_iterate( 
          information,
          (unsigned32 (*)(Objects_Control *, void *))callback,
	  arg,
	  break_on_error );
      if( result && break_on_error )
        return result;
    }
  }
  
  return 0;
}
