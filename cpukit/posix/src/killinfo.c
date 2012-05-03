/*
 *  kill() support routine
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <signal.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/psignal.h>
#include <rtems/seterr.h>
#include <rtems/score/isr.h>

/*
 *  If you enable this, then you get printk() feedback on each path
 *  and the input to the decision that lead to the decision.  Hopefully
 *  this will help in debugging the algorithm that distributes process
 *  signals to individual threads.
 */

/* #define DEBUG_SIGNAL_PROCESSING */
#if defined(DEBUG_SIGNAL_PROCESSING)
  #include <rtems/bspIo.h>
  #define DEBUG_STEP(_x) printk(_x)
#else
  #define DEBUG_STEP(_x)
#endif

/*
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
  uint32_t                     the_api;
  uint32_t                     index;
  uint32_t                     maximum;
  Objects_Information         *the_info;
  Objects_Control            **object_table;
  Thread_Control              *the_thread;
  Thread_Control              *interested;
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
   *  Validate the signal passed.
   */
  if ( !sig )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( !is_valid_signo(sig) )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   *  If the signal is being ignored, then we are out of here.
   */
  if ( _POSIX_signals_Vectors[ sig ].sa_handler == SIG_IGN )
    return 0;

  /*
   *  P1003.1c/Draft 10, p. 33 says that certain signals should always
   *  be directed to the executing thread such as those caused by hardware
   *  faults.
   */
  if ( (sig == SIGFPE) || (sig == SIGILL) || (sig == SIGSEGV ) )
      return pthread_kill( pthread_self(), sig );

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
   *
   *  There is no requirement on the order of threads pending on a sigwait().
   */

  /* XXX violation of visibility -- need to define thread queue support */

  the_chain = &_POSIX_signals_Wait_queue.Queues.Fifo;

  for ( the_node = _Chain_First( the_chain );
        !_Chain_Is_tail( the_chain, the_node ) ;
        the_node = the_node->next ) {

    the_thread = (Thread_Control *)the_node;
    api = the_thread->API_Extensions[ THREAD_API_POSIX ];

    #if defined(DEBUG_SIGNAL_PROCESSING)
      printk( "Waiting Thread=%p option=0x%08x mask=0x%08x blocked=0x%08x\n",
        the_thread, the_thread->Wait.option, mask, api->signals_blocked);
    #endif

    /*
     * Is this thread is actually blocked waiting for the signal?
     */
    if (the_thread->Wait.option & mask)
      goto process_it;

    /*
     * Is this thread is blocked waiting for another signal but has
     * not blocked this one?
     */
    if (~api->signals_blocked & mask)
      goto process_it;
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
  interested = NULL;
  interested_priority = PRIORITY_MAXIMUM + 1;

  for (the_api = OBJECTS_CLASSIC_API; the_api <= OBJECTS_APIS_LAST; the_api++) {

    /*
     *  This can occur when no one is interested and an API is not configured.
     */
    if ( !_Objects_Information_table[ the_api ] )
      continue;

    the_info = _Objects_Information_table[ the_api ][ 1 ];

    #if defined(RTEMS_DEBUG)
      /*
       *  This cannot happen in the current (as of June 2009) implementation
       *  of initialization but at some point, the object information
       *  structure for a particular manager may not be installed.
       */
      if ( !the_info )
        continue;
    #endif

    maximum = the_info->maximum;
    object_table = the_info->local_table;

    for ( index = 1 ; index <= maximum ; index++ ) {
      the_thread = (Thread_Control *) object_table[ index ];

      if ( !the_thread )
        continue;

      #if defined(DEBUG_SIGNAL_PROCESSING)
        printk("\n 0x%08x/0x%08x %d/%d 0x%08x 1",
          the_thread->Object.id,
          ((interested) ? interested->Object.id : 0),
          the_thread->current_priority, interested_priority,
          the_thread->current_state
        );
      #endif

      /*
       *  If this thread is of lower priority than the interested thread,
       *  go on to the next thread.
       */
      if ( the_thread->current_priority > interested_priority )
        continue;
      DEBUG_STEP("2");

      /*
       *  If this thread is not interested, then go on to the next thread.
       */
      api = the_thread->API_Extensions[ THREAD_API_POSIX ];

      #if defined(RTEMS_DEBUG)
        if ( !api )
          continue;
      #endif

      if ( !_POSIX_signals_Is_interested( api, mask ) )
        continue;
      DEBUG_STEP("3");

      /*
       *  Now we know the thread under consideration is interested.
       *  If the thread under consideration is of higher priority, then
       *  it becomes the interested thread.
       *
       *  NOTE: We initialized interested_priority to PRIORITY_MAXIMUM + 1
       *        so we never have to worry about deferencing a NULL
       *        interested thread.
       */
      if ( the_thread->current_priority < interested_priority ) {
        interested   = the_thread;
        interested_priority = the_thread->current_priority;
        continue;
      }
      DEBUG_STEP("4");

      /*
       *  Now the thread and the interested thread have the same priority.
       *  We have to sort through the combinations of blocked/not blocked
       *  and blocking interruptibutable by signal.
       *
       *  If the interested thread is ready, don't think about changing.
       */

      if ( interested && !_States_Is_ready( interested->current_state ) ) {
        /* preferred ready over blocked */
        DEBUG_STEP("5");
        if ( _States_Is_ready( the_thread->current_state ) ) {
          interested          = the_thread;
          interested_priority = the_thread->current_priority;
          continue;
        }

        DEBUG_STEP("6");
        /* prefer blocked/interruptible over blocked/not interruptible */
        if ( !_States_Is_interruptible_by_signal(interested->current_state) ) {
          DEBUG_STEP("7");
          if ( _States_Is_interruptible_by_signal(the_thread->current_state) ) {
            DEBUG_STEP("8");
            interested          = the_thread;
            interested_priority = the_thread->current_priority;
            continue;
          }
        }
      }
    }
  }

  if ( interested ) {
    the_thread = interested;
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

  /*
   *  Returns true if the signal was synchronously given to a thread
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
      _Thread_Enable_dispatch();
      rtems_set_errno_and_return_minus_one( EAGAIN );
    }

    psiginfo->Info = *siginfo;

    _Chain_Append( &_POSIX_signals_Siginfo[ sig ], &psiginfo->Node );
  }

  DEBUG_STEP("\n");
  _Thread_Enable_dispatch();
  return 0;
}
