/*
 *  $Id$
 */

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tqdata.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/seterr.h>
#include <rtems/posix/threadsup.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/time.h>

/*** PROCESS WIDE STUFF ****/

sigset_t  _POSIX_signals_Pending;

struct sigaction _POSIX_signals_Vectors[ SIG_ARRAY_MAX ];

Watchdog_Control _POSIX_signals_Alarm_timer;

Thread_queue_Control _POSIX_signals_Wait_queue;

Chain_Control _POSIX_signals_Inactive_siginfo;
Chain_Control _POSIX_signals_Siginfo[ SIG_ARRAY_MAX ];

struct sigaction _POSIX_signals_Default_vectors[ SIG_ARRAY_MAX ] = {
  /* NO SIGNAL 0 */  SIGACTION_IGNORE,
  /* SIGHUP    1 */  SIGACTION_TERMINATE,
  /* SIGINT    2 */  SIGACTION_TERMINATE,
  /* SIGQUIT   3 */  SIGACTION_TERMINATE,
  /* SIGILL    4 */  SIGACTION_TERMINATE,
  /* SIGTRAP   5 */  SIGACTION_TERMINATE,
  /* SIGIOT    6 */  SIGACTION_TERMINATE,
  /* SIGABRT   6     SIGACTION_TERMINATE, -- alias for SIGIOT */
  /* SIGEMT    7 */  SIGACTION_TERMINATE,
  /* SIGFPE    8 */  SIGACTION_TERMINATE,
  /* SIGKILL   9 */  SIGACTION_TERMINATE,
  /* SIGBUS   10 */  SIGACTION_TERMINATE,
  /* SIGSEGV  11 */  SIGACTION_TERMINATE,
  /* SIGSYS   12 */  SIGACTION_TERMINATE,
  /* SIGPIPE  13 */  SIGACTION_TERMINATE,
  /* SIGALRM  14 */  SIGACTION_TERMINATE,
  /* SIGTERM  15 */  SIGACTION_TERMINATE,
  /* SIGUSR1  16 */  SIGACTION_TERMINATE,
  /* SIGUSR2  17 */  SIGACTION_TERMINATE,
  /* SIGRTMIN 18 */  SIGACTION_IGNORE,
  /* SIGRT    19 */  SIGACTION_IGNORE,
  /* SIGRT    20 */  SIGACTION_IGNORE,
  /* SIGRT    21 */  SIGACTION_IGNORE,
  /* SIGRT    22 */  SIGACTION_IGNORE,
  /* SIGRT    23 */  SIGACTION_IGNORE,
  /* SIGRT    24 */  SIGACTION_IGNORE,
  /* SIGRT    25 */  SIGACTION_IGNORE,
  /* SIGRT    26 */  SIGACTION_IGNORE,
  /* SIGRT    27 */  SIGACTION_IGNORE,
  /* SIGRT    28 */  SIGACTION_IGNORE,
  /* SIGRT    29 */  SIGACTION_IGNORE,
  /* SIGRT    30 */  SIGACTION_IGNORE,
  /* SIGRT    31 */  SIGACTION_IGNORE,
  /* SIGRTMAX 32 */  SIGACTION_IGNORE
};

/*PAGE
 *
 *  _POSIX_signals_Abormal_termination_handler
 *
 */

void _POSIX_signals_Abormal_termination_handler( int signo )
{
  exit( 1 );
}

/*PAGE
 *
 *  _POSIX_signals_Unblock_thread
 */

/* XXX this routine could probably be cleaned up */
boolean _POSIX_signals_Unblock_thread( 
  Thread_Control  *the_thread,
  int              signo,
  siginfo_t       *info
)
{
  POSIX_API_Control  *api;
  sigset_t            mask;
  siginfo_t          *the_info = NULL;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  mask = signo_to_mask( signo );

  /*
   *  Is the thread is specifically waiting for a signal?
   */

  if ( _States_Is_interruptible_signal( the_thread->current_state ) ) {

    if ( (the_thread->Wait.option & mask) || (~api->signals_blocked & mask) ) {
      the_thread->Wait.return_code = EINTR;

      the_info = (siginfo_t *) the_thread->Wait.return_argument;

      if ( !info ) {
        the_info->si_signo = signo;
        the_info->si_code = SI_USER;
        the_info->si_value.sival_int = 0;
      } else {
        *the_info = *info;
      }
      
      _Thread_queue_Extract_with_proxy( the_thread );
      return TRUE;
    }

    /*
     *  This should only be reached via pthread_kill().
     */

    return FALSE;
  }

  if ( ~api->signals_blocked & mask ) {
    the_thread->do_post_task_switch_extension = TRUE;

    if ( the_thread->current_state & STATES_INTERRUPTIBLE_BY_SIGNAL ) {
      the_thread->Wait.return_code = EINTR;
      if ( _States_Is_waiting_on_thread_queue(the_thread->current_state) )
        _Thread_queue_Extract_with_proxy( the_thread );
      else if ( _States_Is_delaying(the_thread->current_state)){
        if ( _Watchdog_Is_active( &the_thread->Timer ) )
          (void) _Watchdog_Remove( &the_thread->Timer );
        _Thread_Unblock( the_thread );
      }
    }
  }
  return FALSE;

}

/*PAGE
 *
 *  _POSIX_signals_Set_process_signals
 */
 
void _POSIX_signals_Set_process_signals(
  sigset_t   mask
)
{
  ISR_Level  level;
 
  _ISR_Disable( level );
    if ( !_POSIX_signals_Pending )
      _Thread_Do_post_task_switch_extension++;
    _POSIX_signals_Pending |= mask;
  _ISR_Enable( level );
}

/*PAGE
 *
 *  _POSIX_signals_Clear_process_signals
 */

void _POSIX_signals_Clear_process_signals(
  sigset_t   mask
)
{
  ISR_Level  level;

  _ISR_Disable( level );
    _POSIX_signals_Pending &= ~mask;
    if ( !_POSIX_signals_Pending && _Thread_Do_post_task_switch_extension )
      _Thread_Do_post_task_switch_extension--;
  _ISR_Enable( level );
}

/*PAGE
 *
 *  _POSIX_signals_Clear_signals
 */
 
boolean _POSIX_signals_Clear_signals(
  POSIX_API_Control  *api,
  int                 signo,
  siginfo_t          *info,
  boolean             is_global,
  boolean             check_blocked
)
{
  sigset_t                    mask;
  sigset_t                    signals_blocked;
  ISR_Level                   level;
  boolean                     do_callout;
  POSIX_signals_Siginfo_node *psiginfo;
 
  mask = signo_to_mask( signo );
 
  do_callout = FALSE;
 
  /* set blocked signals based on if checking for them, SIGNAL_ALL_MASK
   * insures that no signals are blocked and all are checked.
   */

  if ( check_blocked )
    signals_blocked = ~api->signals_blocked;
  else
    signals_blocked = SIGNAL_ALL_MASK;

  /* XXX this is not right for siginfo type signals yet */
  /* XXX since they can't be cleared the same way */
 
  _ISR_Disable( level );
    if ( is_global ) {
       if ( mask & (_POSIX_signals_Pending & signals_blocked) ) {
         if ( _POSIX_signals_Vectors[ signo ].sa_flags == SA_SIGINFO ) {
           psiginfo = (POSIX_signals_Siginfo_node *)
             _Chain_Get_unprotected( &_POSIX_signals_Siginfo[ signo ] );
           if ( _Chain_Is_empty( &_POSIX_signals_Siginfo[ signo ] ) )
             _POSIX_signals_Clear_process_signals( mask );
           if ( psiginfo ) {
             *info = psiginfo->Info;
             _Chain_Append_unprotected(
               &_POSIX_signals_Inactive_siginfo,
               &psiginfo->Node
             );
           } else
             do_callout = FALSE;
         } else
           _POSIX_signals_Clear_process_signals( mask );
         do_callout = TRUE;
       }
    } else {
      if ( mask & (api->signals_pending & signals_blocked) ) {
        api->signals_pending &= ~mask;
        do_callout = TRUE;
      }
    }
  _ISR_Enable( level );
  return do_callout; 
}


/*PAGE
 *
 *  _POSIX_signals_Check_signal
 */

boolean _POSIX_signals_Check_signal(
  POSIX_API_Control  *api,
  int                 signo,
  boolean             is_global
)
{
  siginfo_t                   siginfo_struct;
  sigset_t                    saved_signals_blocked;

  if ( ! _POSIX_signals_Clear_signals( api, signo, &siginfo_struct,
                                       is_global, TRUE ) )
    return FALSE;

  /*
   *  Since we made a union of these, only one test is necessary but this is
   *  safer.
   */ 

  assert( _POSIX_signals_Vectors[ signo ].sa_handler ||
          _POSIX_signals_Vectors[ signo ].sa_sigaction );
 
  /*
   *  Just to prevent sending a signal which is currently being ignored.
   */

  if ( _POSIX_signals_Vectors[ signo ].sa_handler == SIG_IGN )
    return FALSE;

  /*
   *  Block the signals requested in sa_mask
   */

  saved_signals_blocked = api->signals_blocked;
  api->signals_blocked |= _POSIX_signals_Vectors[ signo ].sa_mask;

  switch ( _POSIX_signals_Vectors[ signo ].sa_flags ) {
    case SA_SIGINFO:
      /* assert( is_global ); -- not right after timers added */

      (*_POSIX_signals_Vectors[ signo ].sa_sigaction)(
        signo,
        &siginfo_struct,
        NULL        /* context is undefined per 1003.1b-1993, p. 66 */
      );
      break;
    default:
      (*_POSIX_signals_Vectors[ signo ].sa_handler)( signo );
      break;
  }

  /*
   *  Restore the previous set of blocked signals
   */
 
  api->signals_blocked = saved_signals_blocked;

  return TRUE;
}

void _POSIX_signals_Post_switch_extension(
  Thread_Control  *the_thread
)
{
  POSIX_API_Control  *api;
  int                 signo;
  ISR_Level           level;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  /*
   *  If we invoke any user code, there is the possibility that
   *  a new signal has been posted that we should process so we
   *  restart the loop if a signal handler was invoked.
   *
   *  The first thing done is to check there are any signals to be 
   *  processed at all.  No point in doing this loop otherwise.
   */

restart:
  _ISR_Disable( level );
    if ( !(~api->signals_blocked & 
          (api->signals_pending | _POSIX_signals_Pending)) ) {
     _ISR_Enable( level );
     return;
   }
  _ISR_Enable( level );

  for ( signo = SIGRTMIN ; signo <= SIGRTMAX ; signo++ ) {

    if ( _POSIX_signals_Check_signal( api, signo, FALSE ) )
      goto restart;

    if ( _POSIX_signals_Check_signal( api, signo, TRUE ) )
      goto restart;

  }

/* XXX - add __SIGFIRSTNOTRT or something like that to newlib siginfo.h */

  for ( signo = SIGHUP ; signo <= __SIGLASTNOTRT ; signo++ ) {

    if ( _POSIX_signals_Check_signal( api, signo, FALSE ) )
      goto restart;
 
    if ( _POSIX_signals_Check_signal( api, signo, TRUE ) )
      goto restart;

  }

}

/*PAGE
 *
 *  _POSIX_signals_Alarm_TSR
 */
 
void _POSIX_signals_Alarm_TSR(
  Objects_Id      id,
  void           *argument
)
{
  int status;

  status = kill( getpid(), SIGALRM );
  /* XXX can't print from an ISR, should this be fatal? */
  assert( !status );
}

/*PAGE
 *
 *  _POSIX_signals_Manager_Initialization
 */

void _POSIX_signals_Manager_Initialization(
  int  maximum_queued_signals
)
{
  unsigned32 signo;

  /*
   *  Insure we have the same number of vectors and default vector entries
   */

  assert(
   sizeof(_POSIX_signals_Vectors) == sizeof(_POSIX_signals_Default_vectors)
  );

  memcpy( 
    _POSIX_signals_Vectors,
    _POSIX_signals_Default_vectors,
    sizeof( _POSIX_signals_Vectors )
  );
  
  /*
   *  Initialize the set of pending signals for the entire process
   */

  sigemptyset( &_POSIX_signals_Pending );

  /*
   *  Initialize the timer used to implement alarm().
   */

  _Watchdog_Initialize(
    &_POSIX_signals_Alarm_timer,
    _POSIX_signals_Alarm_TSR,
    0,
    NULL
  );
 
  /*
   *  Initialize the queue we use to block for signals
   */
 
  _Thread_queue_Initialize(
    &_POSIX_signals_Wait_queue,
    OBJECTS_NO_CLASS,
    THREAD_QUEUE_DISCIPLINE_PRIORITY,
    STATES_WAITING_FOR_SIGNAL | STATES_INTERRUPTIBLE_BY_SIGNAL,
    NULL,
    EAGAIN
  );

  /* XXX status codes */

  /* 
   *  Allocate the siginfo pools.
   */

  for ( signo=1 ; signo<= SIGRTMAX ; signo++ )
    _Chain_Initialize_empty( &_POSIX_signals_Siginfo[ signo ] );

  _Chain_Initialize(
    &_POSIX_signals_Inactive_siginfo,
    _Workspace_Allocate_or_fatal_error(
      maximum_queued_signals * sizeof( POSIX_signals_Siginfo_node )
    ),
    maximum_queued_signals,
    sizeof( POSIX_signals_Siginfo_node )
  );
}
