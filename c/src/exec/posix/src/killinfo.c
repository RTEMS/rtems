/*
 *  kill() support routine
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>

#include <rtems/system.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/psignal.h>
#include <rtems/seterr.h>
#include <rtems/score/isr.h>

/*PAGE
 *
 *  3.3.2 Send a Signal to a Process, P1003.1b-1993, p. 68
 *
 *  NOTE: Behavior of kill() depends on _POSIX_SAVED_IDS.
 */

#define _POSIX_signals_Is_interested( _api, _mask ) \
  ( ~(_api)->signals_blocked & (_mask) )
          
int killinfo(
  pid_t               pid,
  int                 sig,
  const union sigval *value
)
{
  sigset_t                     mask;
  POSIX_API_Control           *api;
  unsigned32                   the_class;
  unsigned32                   index;
  unsigned32                   maximum;
  Objects_Information         *the_info;
  Objects_Control            **object_table;
  Thread_Control              *the_thread;
  Thread_Control              *interested_thread;
  Priority_Control             interested_priority;
  Chain_Control               *the_chain;
  Chain_Node                  *the_node;
  siginfo_t                    siginfo_struct;
  siginfo_t                   *siginfo;
  POSIX_signals_Siginfo_node  *psiginfo;
 
  /*
   *  Only supported for the "calling process" (i.e. this node).
   */
 
  if ( pid != getpid() )
    rtems_set_errno_and_return_minus_one( ESRCH );

  /*
   *  Validate the signal passed if not 0.
   */
 
  if ( sig && !is_valid_signo(sig) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /*
   *  If the signal is being ignored, then we are out of here.
   */

  if ( !sig || _POSIX_signals_Vectors[ sig ].sa_handler == SIG_IGN ) {
    return 0;
  }

  /*
   *  P1003.1c/Draft 10, p. 33 says that certain signals should always 
   *  be directed to the executing thread such as those caused by hardware
   *  faults.
   */

  switch ( sig ) {
    case SIGFPE:
    case SIGILL:
    case SIGSEGV:
      return pthread_kill( pthread_self(), sig );
    default:
      break;
  }

  mask = signo_to_mask( sig );

  /*
   *  Build up a siginfo structure
   */

  siginfo = &siginfo_struct;
  siginfo->si_signo = sig;
  siginfo->si_code = SI_USER;
  if ( !value ) {
    siginfo->si_value.sival_int = 0;
  } else {
    siginfo->si_value = *value;
  }

  _Thread_Disable_dispatch();

  /*
   *  Is the currently executing thread interested?  If so then it will
   *  get it an execute it as soon as the dispatcher executes.
   */

  the_thread = _Thread_Executing;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];
  if ( _POSIX_signals_Is_interested( api, mask ) ) {
    goto process_it;
  }

  /*
   *  Is an interested thread waiting for this signal (sigwait())?
   */

  /* XXX violation of visibility -- need to define thread queue support */

  for( index=0 ;
       index < TASK_QUEUE_DATA_NUMBER_OF_PRIORITY_HEADERS ;
       index++ ) {

    the_chain = &_POSIX_signals_Wait_queue.Queues.Priority[ index ];
 
    for ( the_node = the_chain->first ;
          !_Chain_Is_tail( the_chain, the_node ) ;
          the_node = the_node->next ) {

      the_thread = (Thread_Control *)the_node;
      api = the_thread->API_Extensions[ THREAD_API_POSIX ];

      if ((the_thread->Wait.option & mask) || (~api->signals_blocked & mask)) {
        goto process_it;
      }

    }
  }

  /*
   *  Is any other thread interested?  The highest priority interested
   *  thread is selected.  In the event of a tie, then the following
   *  additional criteria is used:
   *
   *    + ready thread over blocked
   *    + blocked on call interruptible by signal (can return EINTR)
   *    + blocked on call not interruptible by signal
   *
   *  This looks at every thread in the system regardless of the creating API.
   *
   *  NOTES:
   *
   *    + rtems internal threads do not receive signals.
   */

  interested_thread = NULL;
  interested_priority = PRIORITY_MAXIMUM + 1;

  for ( the_class = OBJECTS_CLASSES_FIRST_THREAD_CLASS;
        the_class <= OBJECTS_CLASSES_LAST_THREAD_CLASS;
        the_class++ ) {

    if ( the_class == OBJECTS_INTERNAL_THREADS )
      continue;

    the_info = _Objects_Information_table[ the_class ];

    if ( !the_info )                        /* manager not installed */
      continue;

    maximum = the_info->maximum;
    object_table = the_info->local_table;

    assert( object_table );                 /* always at least 1 entry */

    for ( index = 1 ; index <= maximum ; index++ ) {
      the_thread = (Thread_Control *) object_table[ index ];

      if ( !the_thread )
        continue;

      /*
       *  If this thread is of lower priority than the interested thread,
       *  go on to the next thread.
       */

      if ( the_thread->current_priority > interested_priority )
        continue;

      /*
       *  If this thread is not interested, then go on to the next thread.
       */

      api = the_thread->API_Extensions[ THREAD_API_POSIX ];

      if ( !api || !_POSIX_signals_Is_interested( api, mask ) )
        continue;

      /*
       *  Now we know the thread under connsideration is interested.
       *  If the thread under consideration is of higher priority, then
       *  it becomes the interested thread.
       */

      if ( the_thread->current_priority < interested_priority ) {
        interested_thread   = the_thread;
        interested_priority = the_thread->current_priority;
        continue;
      }

      /*
       *  Now the thread and the interested thread have the same priority.
       *  If the interested thread is ready, then we don't need to send it
       *  to a blocked thread.
       */

      if ( _States_Is_ready( interested_thread->current_state ) )
        continue;

      /*
       *  Now the interested thread is blocked.
       *  If the thread we are considering is not, the it becomes the 
       *  interested thread.
       */

      if ( _States_Is_ready( the_thread->current_state ) ) {
        interested_thread   = the_thread;
        interested_priority = the_thread->current_priority;
        continue;
      }

      /*
       *  Now we know both threads are blocked.
       *  If the interested thread is interruptible, then just use it.
       */

      /* XXX need a new states macro */
      if ( interested_thread->current_state & STATES_INTERRUPTIBLE_BY_SIGNAL )
        continue;

      /*
       *  Now both threads are blocked and the interested thread is not 
       *  interruptible.
       *  If the thread under consideration is interruptible by a signal,
       *  then it becomes the interested thread.
       */

      /* XXX need a new states macro */
      if ( the_thread->current_state & STATES_INTERRUPTIBLE_BY_SIGNAL ) {
        interested_thread   = the_thread;
        interested_priority = the_thread->current_priority;
      }
    }
  }

  if ( interested_thread ) {
    the_thread = interested_thread;
    goto process_it;
  }

  /*
   *  OK so no threads were interested right now.  It will be left on the
   *  global pending until a thread receives it.  The global set of threads
   *  can change interest in this signal in one of the following ways:
   *
   *    + a thread is created with the signal unblocked,
   *    + pthread_sigmask() unblocks the signal,
   *    + sigprocmask() unblocks the signal, OR
   *    + sigaction() which changes the handler to SIG_IGN. 
   */

  the_thread = NULL;
  goto post_process_signal;

  /*
   *  We found a thread which was interested, so now we mark that this 
   *  thread needs to do the post context switch extension so it can 
   *  evaluate the signals pending.
   */

process_it:
  
  the_thread->do_post_task_switch_extension = TRUE;

  /*
   *  Returns TRUE if the signal was synchronously given to a thread
   *  blocked waiting for the signal.
   */

  if ( _POSIX_signals_Unblock_thread( the_thread, sig, siginfo ) ) {
    _Thread_Enable_dispatch();
    return 0;
  }

post_process_signal:

  /*
   *  We may have woken up a thread but we definitely need to post the
   *  signal to the process wide information set.
   */

  _POSIX_signals_Set_process_signals( mask );

  if ( _POSIX_signals_Vectors[ sig ].sa_flags == SA_SIGINFO ) {

    psiginfo = (POSIX_signals_Siginfo_node *)
               _Chain_Get( &_POSIX_signals_Inactive_siginfo );
    if ( !psiginfo ) {
      rtems_set_errno_and_return_minus_one( EAGAIN );
    }

    psiginfo->Info = *siginfo;

    _Chain_Append( &_POSIX_signals_Siginfo[ sig ], &psiginfo->Node );
  }

  _Thread_Enable_dispatch();
  return 0;
}
