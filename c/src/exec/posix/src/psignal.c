/* signal.c
 *
 */

#include <signal.h>

#include <rtems/score/thread.h>

#ifdef NOT_IMPLEMENTED_YET

/*
 *  3.3.2 Send a Signal to a Process, P1003.1b-1993, p. 68
 *
 *  NOTE: Behavior of kill() depends on _POSIX_SAVED_IDS.
 */

int kill(
  int   pid_t,
  int   sig
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.3.3 Manipulate Signal Sets, P1003.1b-1993, p. 69
 */

int sigemptyset(
  sigset_t   *set
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.3.3 Manipulate Signal Sets, P1003.1b-1993, p. 69
 */

int sigfillset(
  sigset_t   *set
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.3.3 Manipulate Signal Sets, P1003.1b-1993, p. 69
 */

int sigaddset(
  sigset_t   *set,
  int         signo
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.3.3 Manipulate Signal Sets, P1003.1b-1993, p. 69
 */

int sigdelset(
  sigset_t   *set,
  int         signo
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.3.3 Manipulate Signal Sets, P1003.1b-1993, p. 69
 */

int sigismembet(
  const sigset_t   *set,
  int               signo
)
{
  return POSIX_NOT_IMPLEMENTED();
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
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  3.3.5 Examine and Change Blocked Signals, P1003.1b-1993, p. 73
 *
 *  NOTE: P1003.1c/D10, p. 37 adds pthread_sigmask().
 */

int sigprocmask(
  int               how,
  const sigset_t   *set,
  sigset_t         *oset
)
{
  return POSIX_NOT_IMPLEMENTED();
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
 *  3.4.2 Suspend Process Execution, P1003.1b-1993, p. 80
 */

int pause( void )
{
  return POSIX_NOT_IMPLEMENTED();
}

#endif

/*
 *  3.4.3 Delay Process Execution, P1003.1b-1993, p. 73
 */

unsigned int sleep(
  unsigned int seconds
)
{
  _Thread_Disable_dispatch();
    _Thread_Set_state( _Thread_Executing, STATES_WAITING_FOR_TIME );
    _Watchdog_Initialize(
      &_Thread_Executing->Timer,
      _Thread_Delay_ended,          /* XXX may need to be POSIX specific */
      _Thread_Executing->Object.id,
      NULL
    );
    _Watchdog_Insert_seconds(
      &_Thread_Executing->Timer,
      seconds,
      WATCHDOG_ACTIVATE_NOW
    );
  _Thread_Enable_dispatch();
  return 0;                       /* XXX should account for signal */
}
