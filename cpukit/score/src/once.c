/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/onceimpl.h>
#include <rtems/score/apimutex.h>

#include <errno.h>

#define ONCE_STATE_NOT_RUN  0
#define ONCE_STATE_RUNNING  1
#define ONCE_STATE_COMPLETE 2

int _Once( unsigned char *once_state, void ( *init_routine )( void ) )
{
  int eno = 0;

  if ( *once_state != ONCE_STATE_COMPLETE ) {
    _Once_Lock();

    /*
     * Getting to here means the once_control is locked so we have:
     *  1. The init has not run and the state is ONCE_STATE_NOT_RUN.
     *  2. The init has finished and the state is ONCE_STATE_COMPLETE (already
     *     caught by the previous if).
     *  3. The init is being run by this thread and the state
     *     ONCE_STATE_RUNNING so we are nesting. This is an error.
     */

    switch ( *once_state ) {
      case ONCE_STATE_NOT_RUN:
        *once_state = ONCE_STATE_RUNNING;
        ( *init_routine )();
        *once_state = ONCE_STATE_COMPLETE;
        break;
      case ONCE_STATE_RUNNING:
        eno = EINVAL;
        break;
      default:
        break;
    }

    _Once_Unlock();
  }

  return eno;
}

static API_Mutex_Control _Once_Mutex = API_MUTEX_INITIALIZER( "_Once" );

void _Once_Lock( void )
{
  _API_Mutex_Lock( &_Once_Mutex );
}

void _Once_Unlock( void )
{
  _API_Mutex_Unlock( &_Once_Mutex );
}
