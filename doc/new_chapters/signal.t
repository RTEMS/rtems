@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Signal Manager

@section Introduction

The signal manager ...

The directives provided by the signal manager are:

@itemize @bullet
@item @code{sigaddset} - 
@item @code{sigdelset} - 
@item @code{sigfillset} - 
@item @code{sigismember} - 
@item @code{sigemptyset} - 
@item @code{sigaction} - 
@item @code{pthread_kill} - 
@item @code{sigprocmask} - 
@item @code{pthread_sigmask} - 
@item @code{kill} - 
@item @code{sigpending} - 
@item @code{sigsuspend} - 
@item @code{pause} - 
@item @code{sigwait} - 
@item @code{sigwaitinfo} - 
@item @code{sigtimedwait} - 
@item @code{sigqueue} - 
@item @code{alarm} - 
@end itemize

@section Background

@subsection Signal Delivery

Signals directed at a thread are delivered to the specified thread.

Signals directed at a process are delivered to a thread which is selected
based on the following algorithm:

@enumerate
@item If the action for this signal is currently SIG_IGN, then the signal
is simply ignored.

@item If the currently executing thread has the signal unblocked, then
the signal is delivered to it.

@item If any threads are currently blocked waiting for this signal 
(sigwait()), then the signal is delivered to the highest priority 
thread waiting for this signal.

@item If any other threads are willing to accept delivery of the signal, then
the signal is delivered to the highest priority thread of this set.  In the
event, multiple threads of the same priority are willing to accept this
signal, then priority is given first to ready threads, then to threads
blocked on calls which may be interrupted, and finally to threads blocked
on non-interruptible calls.

@item In the event the signal still can not be delivered, then it is left
pending.  The first thread to unblock the signal (sigprocmask() or
pthread_sigprocmask()) or to wait for this signal (sigwait()) will be
the recipient of the signal.

@end enumerate

@section Operations

@section Directives

This section details the signal manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection sigaddset

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigaddset(
  sigset_t   *set,
  int         signo
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection sigdelset

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigdelset(
  sigset_t   *set,
  int         signo
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection sigfillset

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigfillset(
  sigset_t   *set
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection sigismember

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigismember(
  const sigset_t   *set,
  int               signo
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection sigemptyset

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigemptyset(
  sigset_t   *set
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection sigaction

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigaction(
  int                     sig,
  const struct sigaction *act,
  struct sigaction       *oact
);
@end example

@subheading STATUS CODES:

@table @b
@item EINVAL
Invalid argument passed.

@item ENOTSUP
Realtime Signals Extension option not supported.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

The signal number cannot be SIGKILL.
@page
@subsection pthread_kill

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int pthread_kill(
  pthread_t   thread,
  int         sig
);
@end example

@subheading STATUS CODES:
@table @b
@item ESRCH
The thread indicated by the parameter thread is invalid.

@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection sigprocmask
 
@subheading CALLING SEQUENCE:
 
@example
#include <signal.h>
 
int sigprocmask(
  int               how,
  const sigset_t   *set,
  sigset_t         *oset
);
@end example
 
@subheading STATUS CODES:
@table @b
@item EINVAL
Invalid argument passed.
 
@end table
 
@subheading DESCRIPTION:
 
@subheading NOTES:
 

@page
@subsection pthread_sigmask

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int pthread_sigmask(
  int               how,
  const sigset_t   *set,
  sigset_t         *oset
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

@subheading NOTES:


@page
@subsection kill

@subheading CALLING SEQUENCE:

@example
#include <sys/types.h>
#include <signal.h>

int kill(
  pid_t pid,
  int   sig
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
Invalid argument passed.

@item EPERM
Process does not have permission to send the signal to any receiving process.

@item ESRCH
The process indicated by the parameter pid is invalid.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

 
@page
@subsection sigpending
 
@subheading CALLING SEQUENCE:
 
@example
#include <signal.h>
 
int sigpending(
  const sigset_t  *set
);
@end example
 
@subheading STATUS CODES:

On error, this routine returns -1 and sets errno to one of the following:
 
@table @b
@item EFAULT
Invalid address for set.

@end table

@subheading DESCRIPTION:
 
@subheading NOTES:

@page
@subsection sigsuspend
 
@subheading CALLING SEQUENCE:
 
@example
#include <signal.h>
 
int sigsuspend(
  const sigset_t  *sigmask
);
@end example
 
@subheading STATUS CODES:
@table @b
Returns -1 and sets errno.

@item EINTR
Signal interrupted this function.
 
@end table
 
@subheading DESCRIPTION:
 
@subheading NOTES:

@page
@subsection pause
 
@subheading CALLING SEQUENCE:
 
@example
#include <signal.h>
 
int pause( void );
@end example
 
@subheading STATUS CODES:
@table @b
Returns -1 and sets errno.
 
@item EINTR
Signal interrupted this function.
 
@end table
 
@subheading DESCRIPTION:
 
@subheading NOTES:
 
@page
@subsection sigwait

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigwait(
  const sigset_t  *set,
  int             *sig
);
@end example

@subheading STATUS CODES:
@table @b
@item EINVAL
Invalid argument passed.

@item EINTR
Signal interrupted this function.

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection sigwaitinfo
 
@subheading CALLING SEQUENCE:
 
@example
#include <signal.h>
 
int sigwaitinfo(
  const sigset_t  *set,
  siginfo_t       *info
);
@end example
 
@subheading STATUS CODES:
@table @b
@item EINTR
Signal interrupted this function.
 
@end table
 
@subheading DESCRIPTION:
 
@subheading NOTES:

@page
@subsection sigtimedwait
 
@subheading CALLING SEQUENCE:
 
@example
#include <signal.h>
 
int sigtimedwait(
  const sigset_t         *set,
  siginfo_t              *info,
  const struct timespec  *timeout
);
@end example
 
@subheading STATUS CODES:
@table @b
@item EAGAIN
Timed out while waiting for the specified signal set.
 
@item EINVAL
Nanoseconds field of the timeout argument is invalid.
 
@item EINTR
Signal interrupted this function.
 
@end table
 
@subheading DESCRIPTION:
 
@subheading NOTES:

If timeout is NULL, then the thread will wait forever for the specified 
signal set.

@page
@subsection sigqueue
 
@subheading CALLING SEQUENCE:
 
@example
#include <signal.h>
 
int sigqueue(
  pid_t               pid,
  int                 signo,
  const union sigval  value
);
@end example
 
@subheading STATUS CODES:

@table @b

@item EAGAIN
No resources available to queue the signal.  The process has already
queued SIGQUEUE_MAX signals that are still pending at the receiver
or the systemwide resource limit has been exceeded.
 
@item EINVAL
The value of the signo argument is an invalid or unsupported signal
number.
 
@item EPERM
The process does not have the appropriate privilege to send the signal
to the receiving process.

@item ESRCH
The process pid does not exist.
 
@end table
 
@subheading DESCRIPTION:
 
@subheading NOTES:


@page
@subsection alarm
 
@subheading CALLING SEQUENCE:
 
@example
#include <signal.h>
 
unsigned int alarm(
  unsigned int  seconds
);
@end example
 
@subheading STATUS CODES:

If there was a previous alarm() request with time remaining, then this routine
returns the number of seconds until that outstanding alarm would have fired.
If no previous alarm() request was outstanding, then zero is returned.
 
@subheading DESCRIPTION:
 
@subheading NOTES:


