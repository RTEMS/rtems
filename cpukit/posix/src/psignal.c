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
#include <rtems/posix/pthread.h>
#include <rtems/posix/time.h>

/*
 *  Currently 32 signals numbered 1-32 are defined
 */

#define SIGNAL_EMPTY_MASK  0x00000000
#define SIGNAL_ALL_MASK    0xffffffff

#define signo_to_mask( _sig ) (1 << ((_sig) - 1))

#define is_valid_signo( _sig ) \
  ((_sig) >= 1 && (_sig) <= 32 )

/*** PROCESS WIDE STUFF ****/

sigset_t  _POSIX_signals_Pending;

void _POSIX_signals_Abormal_termination_handler( int signo )
{
  exit( 1 );
}

#define _POSIX_signals_Stop_handler NULL
#define _POSIX_signals_Continue_handler NULL

#define SIGACTION_TERMINATE \
  { 0, SIGNAL_ALL_MASK, {_POSIX_signals_Abormal_termination_handler} }
#define SIGACTION_IGNORE \
  { 0, SIGNAL_ALL_MASK, {SIG_IGN} }
#define SIGACTION_STOP \
  { 0, SIGNAL_ALL_MASK, {_POSIX_signals_Stop_handler} }
#define SIGACTION_CONTINUE \
  { 0, SIGNAL_ALL_MASK, {_POSIX_signals_Continue_handler} }

#define SIG_ARRAY_MAX  (SIGRTMAX + 1)
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

struct sigaction _POSIX_signals_Vectors[ SIG_ARRAY_MAX ];

Watchdog_Control _POSIX_signals_Alarm_timer;

Thread_queue_Control _POSIX_signals_Wait_queue;

typedef struct {
  Chain_Node  Node;
  siginfo_t   Info;
}  POSIX_signals_Siginfo_node;

Chain_Control _POSIX_signals_Inactive_siginfo;
Chain_Control _POSIX_signals_Siginfo[ SIG_ARRAY_MAX ];

/*PAGE
 *
 *  XXX - move these
 */

#define _States_Is_interruptible_signal( _states ) \
  ( ((_states) & \
    (STATES_WAITING_FOR_SIGNAL|STATES_INTERRUPTIBLE_BY_SIGNAL)) == \
      (STATES_WAITING_FOR_SIGNAL|STATES_INTERRUPTIBLE_BY_SIGNAL))

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
    if ( !_POSIX_signals_Pending )
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
  ISR_Level                   level;
  boolean                     do_callout;
  POSIX_signals_Siginfo_node *psiginfo;
 
  mask = signo_to_mask( signo );
 
  do_callout = FALSE;
 
  /* XXX this is not right for siginfo type signals yet */
  /* XXX since they can't be cleared the same way */
 
  _ISR_Disable( level );
    if ( is_global ) {
       if ( mask & (_POSIX_signals_Pending & 
                       (~api->signals_blocked & check_blocked) ) ) {
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
      if ( mask & (api->signals_pending & 
                      (~api->signals_blocked & check_blocked) ) ) {
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
      assert( is_global );

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

  for ( signo = SIGABRT ; signo <= __SIGLASTNOTRT ; signo++ ) {

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

/*
 *  3.3.3 Manipulate Signal Sets, P1003.1b-1993, p. 69
 */

int sigemptyset(
  sigset_t   *set
)
{
  if ( !set )
    set_errno_and_return_minus_one( EFAULT );

  *set = 0;
  return 0;
}

/*
 *  3.3.3 Manipulate Signal Sets, P1003.1b-1993, p. 69
 */

int sigfillset(
  sigset_t   *set
)
{
  if ( !set )
    set_errno_and_return_minus_one( EFAULT );

  *set = SIGNAL_ALL_MASK;
  return 0;
}

/*
 *  3.3.3 Manipulate Signal Sets, P1003.1b-1993, p. 69
 */

int sigaddset(
  sigset_t   *set,
  int         signo
)
{
  if ( !set )
    set_errno_and_return_minus_one( EFAULT );

  if ( !is_valid_signo(signo) )
    set_errno_and_return_minus_one( EINVAL );

  *set |= signo_to_mask(signo);
  return 0;
}

/*
 *  3.3.3 Manipulate Signal Sets, P1003.1b-1993, p. 69
 */

int sigdelset(
  sigset_t   *set,
  int         signo
)
{
  if ( !set )
    set_errno_and_return_minus_one( EFAULT );
 
  if ( !is_valid_signo(signo) )
    set_errno_and_return_minus_one( EINVAL );
 
  *set &= ~signo_to_mask(signo);
  return 0;
}

/*
 *  3.3.3 Manipulate Signal Sets, P1003.1b-1993, p. 69
 */

int sigismember(
  const sigset_t   *set,
  int               signo
)
{
  if ( !set ) 
    set_errno_and_return_minus_one( EFAULT );
 
  if ( !is_valid_signo(signo) )
    set_errno_and_return_minus_one( EINVAL );
 
  if ( *set & signo_to_mask(signo) )
    return 1;

  return 0;
}

/*
 *  3.3.4 Examine and Change Signal Action, P1003.1b-1993, p. 70
 */

int sigaction(
  int                     sig,
  const struct sigaction *act,
  struct sigaction       *oact
)
{
  ISR_Level     level;

  if ( !is_valid_signo(sig) )
    set_errno_and_return_minus_one( EINVAL );
  
  if ( oact )
    *oact = _POSIX_signals_Vectors[ sig ];

  /* 
   *  Some signals cannot be ignored (P1003.1b-1993, pp. 70-72 and references.
   *
   *  NOTE: Solaris documentation claims to "silently enforce" this which
   *        contradicts the POSIX specification.
   */

  if ( sig == SIGKILL )
    set_errno_and_return_minus_one( EINVAL );
  
  /*
   *  Evaluate the new action structure and set the global signal vector
   *  appropriately.
   */

  if ( act ) {

    /*
     *  Unless the user is installing the default signal actions, then
     *  we can just copy the provided sigaction structure into the vectors.
     */

    _ISR_Disable( level );
      if ( act->sa_handler == SIG_DFL ) {
        _POSIX_signals_Vectors[ sig ] = _POSIX_signals_Default_vectors[ sig ];
      } else {
         _POSIX_signals_Clear_process_signals( signo_to_mask(sig) );
         _POSIX_signals_Vectors[ sig ] = *act;
      }
    _ISR_Enable( level );
  }

  /*
   *  No need to evaluate or dispatch because:
   *
   *    + If we were ignoring the signal before, none could be pending 
   *      now (signals not posted when SIG_IGN).
   *    + If we are now ignoring a signal that was previously pending,
   *      we clear the pending signal indicator.
   */

  return 0;
}

/*
 *  3.3.5 Examine and Change Blocked Signals, P1003.1b-1993, p. 73
 *
 *  NOTE: P1003.1c/D10, p. 37 adds pthread_sigmask().
 *
 */

int sigprocmask(
  int               how,
  const sigset_t   *set,
  sigset_t         *oset
)
{
  /*
   *  P1003.1c/Draft 10, p. 38 maps sigprocmask to pthread_sigmask.
   */

  return pthread_sigmask( how, set, oset );
}

/*
 *  3.3.5 Examine and Change Blocked Signals, P1003.1b-1993, p. 73
 *
 *  NOTE: P1003.1c/D10, p. 37 adds pthread_sigmask().
 */

int pthread_sigmask(
  int               how,
  const sigset_t   *set,
  sigset_t         *oset
)
{
  POSIX_API_Control  *api;

  if ( !set && !oset )
    set_errno_and_return_minus_one( EFAULT );

  api = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];

  if ( oset )
    *oset = api->signals_blocked;
 
  if ( !set )
    set_errno_and_return_minus_one( EFAULT );

  switch ( how ) {
    case SIG_BLOCK:
      api->signals_blocked |= *set;
      break;
    case SIG_UNBLOCK:
      api->signals_blocked &= ~*set;
      break;
    case SIG_SETMASK:
      api->signals_blocked = *set;
      break;
    default:
      set_errno_and_return_minus_one( EINVAL );
  }

  /* XXX are there critical section problems here? */

  /* XXX evaluate the new set */

  if ( ~api->signals_blocked & 
       (api->signals_pending | _POSIX_signals_Pending) ) {
    _Thread_Executing->do_post_task_switch_extension = TRUE;
    _Thread_Dispatch();
  }

  return 0;
}

/*
 *  3.3.6 Examine Pending Signals, P1003.1b-1993, p. 75
 */

int sigpending(
  sigset_t  *set
)
{
  POSIX_API_Control  *api;
 
  if ( !set )
    set_errno_and_return_minus_one( EFAULT );
 
  api = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
 
  *set = api->signals_pending | _POSIX_signals_Pending;
 
  return 0;
}

/*
 *  3.3.7 Wait for a Signal, P1003.1b-1993, p. 75
 */

int sigsuspend(
  const sigset_t  *sigmask
)
{
  sigset_t            saved_signals_blocked;
  sigset_t            all_signals;
  int                 status;
  POSIX_API_Control  *api;

  api = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];

  saved_signals_blocked = api->signals_blocked;
 
  (void) sigfillset( &all_signals );

  status = sigtimedwait( &all_signals, NULL, NULL );

  api->signals_blocked = saved_signals_blocked;

  return status;
}

/*
 *  3.3.8 Synchronously Accept a Signal, P1003.1b-1993, p. 76
 *
 *  NOTE: P1003.1c/D10, p. 39 adds sigwait().
 */

int sigwaitinfo(
  const sigset_t  *set,
  siginfo_t       *info
)
{
  return sigtimedwait( set, info, NULL );
}

/*
 *  3.3.8 Synchronously Accept a Signal, P1003.1b-1993, p. 76
 *
 *  NOTE: P1003.1c/D10, p. 39 adds sigwait().
 */

int _POSIX_signals_Get_highest( 
  sigset_t   set
)
{
  int signo;
 
  for ( signo = SIGRTMIN ; signo <= SIGRTMAX ; signo++ ) {
    if ( set & signo_to_mask( signo ) )
      return signo;
  }
 
  for ( signo = SIGABRT ; signo <= __SIGLASTNOTRT ; signo++ ) {
    if ( set & signo_to_mask( signo ) )
      return signo;
  }

  return 0;
}
  

int sigtimedwait(
  const sigset_t         *set,
  siginfo_t              *info,
  const struct timespec  *timeout
)
{
  Thread_Control    *the_thread;
  POSIX_API_Control *api;
  Watchdog_Interval  interval;
  siginfo_t          signal_information;
  siginfo_t         *the_info;
  int                signo;
  
  the_info = ( info ) ? info : &signal_information;

  the_thread = _Thread_Executing;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  /*
   *  What if they are already pending?
   */

  /* API signals pending? */

  if ( *set & api->signals_pending ) {
    /* XXX real info later */
    the_info->si_signo = _POSIX_signals_Get_highest( api->signals_pending );
    _POSIX_signals_Clear_signals( api, the_info->si_signo, the_info,
                                  FALSE, FALSE );
    the_info->si_code = SI_USER;
    the_info->si_value.sival_int = 0;
    return the_info->si_signo;
  }

  /* Process pending signals? */

  if ( *set & _POSIX_signals_Pending) {
    signo = _POSIX_signals_Get_highest( _POSIX_signals_Pending );
    _POSIX_signals_Clear_signals( api, signo, the_info, TRUE, FALSE );

    if ( !info ) {
      the_info->si_signo = signo;
      the_info->si_code = SI_USER;
      the_info->si_value.sival_int = 0;
    }
  }

  interval = 0;
  if ( timeout ) {

    if (timeout->tv_nsec < 0 || timeout->tv_nsec >= TOD_NANOSECONDS_PER_SECOND)
      set_errno_and_return_minus_one( EINVAL );

    interval = _POSIX_Timespec_to_interval( timeout );
  }

  the_info->si_signo = -1;

  _Thread_Disable_dispatch();
    the_thread->Wait.queue           = &_POSIX_signals_Wait_queue;
    the_thread->Wait.return_code     = EINTR;
    the_thread->Wait.option          = *set;
    the_thread->Wait.return_argument = (void *) the_info;
    _Thread_queue_Enter_critical_section( &_POSIX_signals_Wait_queue ); 
    _Thread_queue_Enqueue( &_POSIX_signals_Wait_queue, interval ); 
  _Thread_Enable_dispatch();

  errno = _Thread_Executing->Wait.return_code;
  return the_info->si_signo;
}

/*
 *  3.3.8 Synchronously Accept a Signal, P1003.1b-1993, p. 76
 *
 *  NOTE: P1003.1c/D10, p. 39 adds sigwait().
 */

int sigwait(
  const sigset_t  *set,
  int             *sig
)
{
  int status;

  status = sigtimedwait( set, NULL, NULL );

  if ( status != -1 ) {
    if ( sig )
      *sig = status;
    return 0;
  }

  return errno;
}

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
 
  if( pid != getpid() )
    set_errno_and_return_minus_one( ESRCH );

  /*
   *  If the signal is being ignored, then we are out of here.
   */

  if ( !sig || _POSIX_signals_Vectors[ sig ].sa_handler == SIG_IGN )
    return 0;

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
    if ( !psiginfo )
      set_errno_and_return_minus_one( EAGAIN );

    psiginfo->Info = *siginfo;

    _Chain_Append( &_POSIX_signals_Siginfo[ sig ], &psiginfo->Node );
  }

  _Thread_Enable_dispatch();
  return 0;
}

/*PAGE
 *
 *  3.3.2 Send a Signal to a Process, P1003.1b-1993, p. 68
 *
 *  NOTE: Behavior of kill() depends on _POSIX_SAVED_IDS.
 */

int kill(
  pid_t pid,
  int   sig
)
{
  return killinfo( pid, sig, NULL );
}

/*
 *  3.3.9 Queue a Signal to a Process, P1003.1b-1993, p. 78
 */

int sigqueue(
  pid_t               pid,
  int                 signo,
  const union sigval  value
)
{
  return killinfo( pid, signo, &value );
}

/*
 *  3.3.10 Send a Signal to a Thread, P1003.1c/D10, p. 43
 */

int pthread_kill(
  pthread_t   thread,
  int         sig
)
{
  POSIX_API_Control  *api;
  Thread_Control     *the_thread;
  Objects_Locations  location;

  if ( sig && !is_valid_signo(sig) )
    set_errno_and_return_minus_one( EINVAL );

  if ( _POSIX_signals_Vectors[ sig ].sa_handler == SIG_IGN )
    return 0;

  /*
   *  RTEMS does not support sending  a siginfo signal to a specific thread.
   */

  if ( _POSIX_signals_Vectors[ sig ].sa_flags == SA_SIGINFO )
    return ENOSYS;

  the_thread = _POSIX_Threads_Get( thread, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      return ESRCH;
    case OBJECTS_LOCAL:
      /*
       *  If sig == 0 then just validate arguments
       */

      api = the_thread->API_Extensions[ THREAD_API_POSIX ];

      if ( sig ) {

        /* XXX critical section */

        api->signals_pending |= signo_to_mask( sig );

        (void) _POSIX_signals_Unblock_thread( the_thread, sig, NULL );
    }
    _Thread_Enable_dispatch();
    return 0;
  }

  return POSIX_BOTTOM_REACHED();
}

/*
 *  3.4.1 Schedule Alarm, P1003.1b-1993, p. 79
 */

Watchdog_Control _POSIX_signals_Alarm_timer;

unsigned int alarm(
  unsigned int seconds
)
{
  unsigned int      remaining = 0;
  Watchdog_Control *the_timer;

  the_timer = &_POSIX_signals_Alarm_timer;

  switch ( _Watchdog_Remove( the_timer ) ) {
    case WATCHDOG_INACTIVE:
    case WATCHDOG_BEING_INSERTED:
      break;
 
    case WATCHDOG_ACTIVE:
    case WATCHDOG_REMOVE_IT:
      remaining = the_timer->initial - 
                  (the_timer->stop_time - the_timer->start_time);
      break;
  }

  _Watchdog_Insert_seconds( the_timer, seconds );

  return remaining;
}

/*
 *  3.4.2 Suspend Process Execution, P1003.1b-1993, p. 81
 */

int pause( void )
{
  sigset_t  all_signals;
  int       status;
 
  (void) sigfillset( &all_signals );
 
  status = sigtimedwait( &all_signals, NULL, NULL );
 
  return status;
}
