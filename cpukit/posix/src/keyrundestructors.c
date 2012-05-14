/*
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/posix/key.h>

/*
 *  _POSIX_Keys_Run_destructors
 *
 *  17.1.1 Thread-Specific Data Key Create, P1003.1c/Draft 10, p. 163
 *
 *  NOTE:  This is the routine executed when a thread exits to
 *         run through all the keys and do the destructor action.
 */

void _POSIX_Keys_Run_destructors(
  Thread_Control *thread
)
{
  Objects_Maximum thread_index = _Objects_Get_index( thread->Object.id );
  Objects_APIs thread_api = _Objects_Get_API( thread->Object.id );
  bool done = false;

  /*
   *  The standard allows one to avoid a potential infinite loop and limit the
   *  number of iterations.  An infinite loop may happen if destructors set
   *  thread specific data.  This can be considered dubious.
   *
   *  Reference: 17.1.1.2 P1003.1c/Draft 10, p. 163, line 99.
   */
  while ( !done ) {
    Objects_Maximum index = 0;
    Objects_Maximum max = _POSIX_Keys_Information.maximum;

    done = true;

    for ( index = 1 ; index <= max ; ++index ) {
      POSIX_Keys_Control *key = (POSIX_Keys_Control *)
        _POSIX_Keys_Information.local_table [ index ];

      if ( key != NULL && key->destructor != NULL ) {
        void *value = key->Values [ thread_api ][ thread_index ];

        if ( value != NULL ) {
          key->Values [ thread_api ][ thread_index ] = NULL;
          (*key->destructor)( value );
          done = false;
        }
      }
    }
  }
}
