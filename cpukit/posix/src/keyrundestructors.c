/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/key.h>

/*PAGE
 *
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
  unsigned32           index;
  unsigned32           pthread_index;
  unsigned32           pthread_class;
  unsigned32           iterations;
  boolean              are_all_null;
  POSIX_Keys_Control  *the_key;
  void                *value;

  pthread_index = _Objects_Get_index( thread->Object.id );
  pthread_class = _Objects_Get_class( thread->Object.id );

  iterations = 0;

  for ( ; ; ) {

    are_all_null = TRUE;

    for ( index=1 ; index <= _POSIX_Keys_Information.maximum ; index++ ) {

      the_key = (POSIX_Keys_Control *) 
        _POSIX_Keys_Information.local_table[ index ];
      
      if ( the_key && the_key->is_active && the_key->destructor ) {
        value = the_key->Values[ pthread_class ][ pthread_index ];
        if ( value ) {
          (*the_key->destructor)( value );
          if ( the_key->Values[ pthread_class ][ pthread_index ] )
            are_all_null = FALSE;
        }
      }
    }

    if ( are_all_null == TRUE ) 
      return;

    iterations++;

    /*
     *  The standard allows one to not do this and thus go into an infinite
     *  loop.  It seems rude to unnecessarily lock up a system.
     *
     *  Reference: 17.1.1.2 P1003.1c/Draft 10, p. 163, line 99.
     */

    if ( iterations >= PTHREAD_DESTRUCTOR_ITERATIONS )
      return; 
  }
}
