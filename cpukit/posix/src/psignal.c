/*
 *  $Id$
 */

#include <assert.h>
#include <errno.h>
#include <signal.h>

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/posix/seterr.h>
#include <rtems/posix/threadsup.h>

/*
 *  Currently 32 signals numbered 1-32 are defined
 */

#define SIGNAL_EMPTY_MASK  0x00000000
#define SIGNAL_ALL_MASK    0xffffffff

#define signo_to_mask( _sig ) (1 << ((_sig) - 1))

#define is_valid_signo( _sig ) \
  ((signo_to_mask(_sig) & SIGNAL_ALL_MASK) != 0 )

/*** PROCESS WIDE STUFF ****/

sigset_t  _POSIX_signals_Blocked = SIGNAL_EMPTY_MASK;
sigset_t  _POSIX_signals_Pending = SIGNAL_EMPTY_MASK;

struct sigaction _POSIX_signals_Vectors[ SIGRTMAX ];

/*PAGE
 *
 *  _POSIX_signals_Manager_Initialization
 */

void _POSIX_signals_Manager_Initialization( void )
{
  /* XXX install default actions for all vectors */
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
  if ( !act )
    set_errno_and_return_minus_one( EFAULT );

  if ( !is_valid_signo(sig) )
    set_errno_and_return_minus_one( EINVAL );
  
  if ( oact )
    *oact = _POSIX_signals_Vectors[ sig ];

  /* XXX need to interpret some stuff here */

  _POSIX_signals_Vectors[ sig ] = *act;

  return POSIX_NOT_IMPLEMENTED();
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

  /* XXX evaluate the new set */

  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.3.6 Examine Pending Signals, P1003.1b-1993, p. 75
 */

int sigpending(
  sigset_t  *set
)
{
  return POSIX_NOT_IMPLEMENTED();
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

int kill(
  pid_t pid,
  int   sig
)
{
  /*
   *  Only supported for the "calling process" (i.e. this node).
   */
 
  assert( pid == getpid() );

  /* SIGABRT comes from abort via assert */
  if ( sig == SIGABRT ) {
    exit( 1 );
  }
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.3.10 Send a Signal to a Thread, P1003.1c/D10, p. 43
 */

int pthread_kill(
  pthread_t   thread,
  int         sig
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.4.1 Schedule Alarm, P1003.1b-1993, p. 79
 */

unsigned int alarm(
  unsigned int seconds
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.4.2 Suspend Process Execution, P1003.1b-1993, p. 81
 */

int pause( void )
{
  return POSIX_NOT_IMPLEMENTED();
}
