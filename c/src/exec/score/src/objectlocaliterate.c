/*
 *  object iterator
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

/*PAGE
 *
 *  _Objects_Local_iterate
 *
 *  DESCRIPTION:
 *
 *  This function invokes the callback function for each existing object
 *  of the type specified by the information block pointer. Iteration
 *  continues until either all objects have been processed, or, if
 *  break_on_error is TRUE, until an invocation of the callback returns
 *  something other than 0.
 *
 *  Input parameters:
 *    information:
 *      A pointer to an Objects_Information block. Determines the type of
 *      object over which to iterate.
 *    callback:
 *      A pointer to a function with the indicated signature.
 *    arg: 
 *      A pointer to some arbitrary entity. Passed on to the callback.
 *    break_on_error
 *      If TRUE, stop iterating on error.
 *
 *  Output parameters: NONE
 *    But callback may write into space pointed to by arg.
 *
 *  Return value:
 *    0 if successful
 *    Value returned by the callback otherwise.
 */

unsigned32 _Objects_Local_iterate(
  Objects_Information  *information,
  unsigned32          (*callback)(Objects_Control *object, void * arg),
  void                 *arg,
  boolean               break_on_error
)
{
  unsigned32       result;
  unsigned32       i;
  Objects_Control *the_object;

  if ( !information )
    return 0;
    
  if ( !callback )
    return 0;   
    
  for( i = 1; i <= information->maximum; i++ ) {
    the_object = (Objects_Control *)information->local_table[i];
    if( the_object ) {
      result = (*callback)( the_object, arg );
      if ( result && break_on_error )
        return result;
    }
  }
    
  return 0;
}

