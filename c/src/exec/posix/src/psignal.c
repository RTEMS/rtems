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
#include <rtems/posix/seterr.h>
#include <rtems/posix/threadsup.h>
#include <rtems/posix/pthread.h>

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
  /* SIGABRT     */  SIGACTION_TERMINATE,
  /* SIGALRM     */  SIGACTION_TERMINATE,
  /* SIGFPE      */  SIGACTION_TERMINATE,
  /* SIGHUP      */  SIGACTION_TERMINATE,
  /* SIGILL      */  SIGACTION_TERMINATE,
  /* SIGINT      */  SIGACTION_TERMINATE,
  /* SIGKILL     */  SIGACTION_TERMINATE,
  /* SIGPIPE     */  SIGACTION_TERMINATE,
  /* SIGQUIT     */  SIGACTION_TERMINATE,
  /* SIGSEGV     */  SIGACTION_TERMINATE,
  /* SIGTERM     */  SIGACTION_TERMINATE,
  /* SIGUSR1     */  SIGACTION_TERMINATE,
  /* SIGUSR2     */  SIGACTION_TERMINATE,
  /* SIGRTMIN 14 */  SIGACTION_IGNORE,
  /* SIGRT    15 */  SIGACTION_IGNORE,
  /* SIGRT    16 */  SIGACTION_IGNORE,
  /* SIGRT    17 */  SIGACTION_IGNORE,
  /* SIGRT    18 */  SIGACTION_IGNORE,
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

typedef struct {
  Chain_Node  Node;
  siginfo_t   Info;
}  POSIX_signals_Siginfo_node;

Chain_Control _POSIX_signals_Inactive_siginfo;
Chain_Control _POSIX_signals_Siginfo[ SIG_ARRAY_MAX ];

/*PAGE
 *
 *  XXX
 */

boolean _POSIX_signals_Check_signal(
  POSIX_API_Control  *api,
  int                 signo,
  boolean             is_global
)
{
  sigset_t            mask;
  ISR_Level           level;
  boolean             do_callout;
  siginfo_t          *siginfo = NULL;  /* really needs to be set below */
  siginfo_t           siginfo_struct;
  sigset_t            saved_signals_blocked;

  mask = signo_to_mask( signo );

  do_callout = FALSE;

  /* XXX this is not right for siginfo type signals yet */
  /* XXX since they can't be cleared the same way */
  _ISR_Disable( level );
    if ( is_global ) {
       if ( mask & (_POSIX_signals_Pending & ~api->signals_blocked ) ) {
         _POSIX_signals_Pending &= ~mask;
         do_callout = TRUE;
       }
    } else {
      if ( mask & (api->signals_pending & ~api->signals_blocked ) ) {
        api->signals_pending &= ~mask;
        do_callout = TRUE;
      }
    }
  _ISR_Enable( level );

  if ( !do_callout )
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
      assert( 0 );   /* XXX we haven't completely implemented this yet */
      if ( !is_global ) {
        siginfo = &siginfo_struct;
        siginfo->si_signo = signo;
        siginfo->si_code = SI_USER;
        siginfo->si_value.sival_int = 0;
      }
      (*_POSIX_signals_Vectors[ signo ].sa_sigaction)(
        signo,
        siginfo,
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
          (api->signals_pending | _POSIX_signals_Pending)) )
      return;
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
  kill( getpid(), SIGALRM );
}

/*PAGE
 *
 *  _POSIX_signals_Manager_Initialization
 */

void _POSIX_signals_Manager_Initialization( void )
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
   *  XXX Allocate the siginfo pools.
   */

  for ( signo=1 ; signo<= SIGRTMAX ; signo++ )
    _Chain_Initialize_empty( &_POSIX_signals_Siginfo[ signo ] );

  /* XXX especially the inactive pool */
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

    _ISR_Disable( level );
      if ( act->sa_handler == SIG_DFL ) {
        _POSIX_signals_Vectors[ sig ] = _POSIX_signals_Default_vectors[ sig ];
      } else if ( act->sa_handler == SIG_DFL ) {
        _POSIX_signals_Pending &= ~signo_to_mask( sig );
      } else {
        _POSIX_signals_Pending &= ~signo_to_mask( sig );
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
  return POSIX_NOT_IMPLEMENTED();
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
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.3.8 Synchronously Accept a Signal, P1003.1b-1993, p. 76
 *
 *  NOTE: P1003.1c/D10, p. 39 adds sigwait().
 */

int sigtimedwait(
  const sigset_t         *set,
  siginfo_t              *info,
  const struct timespec  *timeout
)
{
  return POSIX_NOT_IMPLEMENTED();
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
  return POSIX_NOT_IMPLEMENTED();
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
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.3.2 Send a Signal to a Process, P1003.1b-1993, p. 68
 *
 *  NOTE: Behavior of kill() depends on _POSIX_SAVED_IDS.
 */

#define _POSIX_signals_Is_interested( _api, _mask ) \
  ( ~(_api)->signals_blocked & (_mask) )
          
int kill(
  pid_t pid,
  int   sig
)
{
  sigset_t              mask;
  POSIX_API_Control    *api;
  unsigned32            the_class;
  unsigned32            index;
  unsigned32            maximum;
  Objects_Information  *the_info;
  Objects_Control     **object_table;
  Thread_Control       *the_thread;
  Thread_Control       *interested_thread;
  Priority_Control      interested_priority;


  /*
   *  Only supported for the "calling process" (i.e. this node).
   */
 
  if( pid != getpid() );
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

  _Thread_Disable_dispatch();

  _POSIX_signals_Pending |= mask;

  /*
   *  Is the currently executing thread interested?
   */

  the_thread = _Thread_Executing;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];
  if ( _POSIX_signals_Is_interested( api, mask ) )
    goto process_it;

  /*
   *  Is an interested thread waiting for this signal (sigwait())?
   */

  /* XXX wait for signal functions need to be done */

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

    if ( !the_info )
      continue;

    maximum = the_info->maximum;
    object_table = the_info->local_table;

    assert( object_table );

    object_table++;                         /* skip entry 0 */

    for ( index = 1 ; index <= maximum ; index++ ) {
      the_thread = (Thread_Control *) object_table++;

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

      if ( !_POSIX_signals_Is_interested( api, mask ) )
        continue;

      /*
       *  If this thread is of higher priority logically and interested, then
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
       *  The interested thread is blocked and the thread we are considering
       *  is not, so it becomes the interested thread.
       */

      if ( _States_Is_ready( the_thread->current_state ) ) {
        interested_thread   = the_thread;
        interested_priority = the_thread->current_priority;
        continue;
      }

      /*
       *  If the interested thread is interruptible, then just use it.
       */

      /* XXX need a new states macro */
      if ( interested_thread->current_state & STATES_INTERRUPTIBLE_BY_SIGNAL )
        continue;

      /*
       *  Both the thread under consideration and the interested thread are
       *  blocked and the interested thread is not interruptible by a signal.
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

  _Thread_Enable_dispatch();
  return 0;

  /*
   *  We found a thread which was interested, so now we mark that this 
   *  thread needs to do the post context switch extension so it can 
   *  evaluate the signals pending.
   */

process_it:
  /* XXX what if the thread is blocked? -- need code from pthread_kill */
  /* XXX and it needs to be in a subroutine */
  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  the_thread->do_post_task_switch_extension = TRUE;

  _Thread_Enable_dispatch();

   /* XXX take this out when a little more confident */
  /* SIGABRT comes from abort via assert and must work no matter what */
  if ( sig == SIGABRT ) {
    exit( 1 );
  }
  return 0;
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

        if ( api->signals_pending & ~api->signals_blocked ) {
          the_thread->do_post_task_switch_extension = TRUE;
 
          /* XXX unblock the task -- this is a kludge -- fix it */

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
  return POSIX_NOT_IMPLEMENTED();
}
