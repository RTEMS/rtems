@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Signal Manager

@section Introduction

The signal manager provides the functionality associated with
the generation, delivery, and management of process-oriented
signals.

The directives provided by the signal manager are:

@itemize @bullet
@item @code{sigaddset} - Add a Signal to a Signal Set
@item @code{sigdelset} - Delete a Signal from a Signal Set
@item @code{sigfillset} - Fill a Signal Set
@item @code{sigismember} - Is Signal a Member of a Signal Set
@item @code{sigemptyset} - Empty a Signal Set
@item @code{sigaction} - Examine and Change Signal Action
@item @code{pthread_kill} - Send a Signal to a Thread
@item @code{sigprocmask} - Examine and Change Process Blocked Signals
@item @code{pthread_sigmask} - Examine and Change Thread Blocked Signals
@item @code{kill} - Send a Signal to a Process
@item @code{sigpending} - Examine Pending Signals
@item @code{sigsuspend} - Wait for a Signal
@item @code{pause} - Suspend Process Execution
@item @code{sigwait} - Synchronously Accept a Signal
@item @code{sigwaitinfo} - Synchronously Accept a Signal
@item @code{sigtimedwait} - Synchronously Accept a Signal with Timeout
@item @code{sigqueue} - Queue a Signal to a Process
@item @code{alarm} - Schedule Alarm
@item @code{ualarm} - Schedule Alarm in Microseconds
@end itemize

@section Background

@subsection Signals

POSIX signals are an asynchronous event mechanism.  Each process
and thread has a set of signals associated with it.  Individual
signals may be enabled (e.g. unmasked) or blocked (e.g. ignored)
on both a per-thread and process level.  Signals which are
enabled have a signal handler associated with them.  When the
signal is generated and conditions are met, then the signal
handler is invoked in the proper process or thread context
asynchronous relative to the logical thread of execution.

If a signal has been blocked when it is generated, then it
is queued and kept pending until the thread or process unblocks
the signal or explicitly checks for it.
Traditional, non-real-time POSIX signals do not queue.  Thus
if a process or thread has blocked a particular signal, then
multiple occurrences of that signal are recorded as a
single occurrence of that signal.
@c TODO: SIGRTMIN and SIGRTMAX ?

One can check for the set of outstanding signals that have been
blocked.   Services are provided to check for outstanding process
or thread directed signals.

@subsection Signal Delivery

Signals which are directed at a thread are delivered to the specified thread.

Signals which are directed at a process are delivered to a thread which
is selected based on the following algorithm:

@enumerate
@item If the action for this signal is currently @code{SIG_IGN},
then the signal is simply ignored.

@item If the currently executing thread has the signal unblocked, then
the signal is delivered to it.

@item If any threads are currently blocked waiting for this signal
(@code{sigwait()}), then the signal is delivered to the highest priority
thread waiting for this signal.

@item If any other threads are willing to accept delivery of the signal, then
the signal is delivered to the highest priority thread of this set. In the
event, multiple threads of the same priority are willing to accept this
signal, then priority is given first to ready threads, then to threads
blocked on calls which may be interrupted, and finally to threads blocked
on non-interruptible calls.

@item In the event the signal still can not be delivered, then it is left
pending. The first thread to unblock the signal (@code{sigprocmask()} or
@code{pthread_sigprocmask()}) or to wait for this signal
(@code{sigwait()}) will be the recipient of the signal.

@end enumerate

@section Operations

@subsection Signal Set Management

Each process and each thread within that process has a set of
individual signals and handlers associated with it.   Services
are provided to construct signal sets for the purposes of building
signal sets -- type @code{sigset_t} -- that are used to
provide arguments to the services that mask, unmask, and
check on pending signals.

@subsection Blocking Until Signal Generation

A thread may block until receipt of a signal.  The "sigwait"
and "pause" families of functions block until the requested
signal is received or if using @code{sigtimedwait()} until the specified
timeout period has elapsed.

@subsection Sending a Signal

This is accomplished
via one of a number of services that sends a signal to either a
process or thread.  Signals may be directed at a process by
the service @code{kill()} or at a thread by the service
@code{pthread_kill()}

@section Directives

This section details the signal manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection sigaddset - Add a Signal to a Signal Set

@findex sigaddset
@cindex  add a signal to a signal set

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigaddset(
  sigset_t *set,
  int       signo
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

This function adds the signal @code{signo} to the specified signal @code{set}.

@subheading NOTES:

The set must be initialized using either @code{sigemptyset} or @code{sigfillset}
before using this function.

@c
@c
@c
@page
@subsection sigdelset - Delete a Signal from a Signal Set

@findex sigdelset
@cindex  delete a signal from a signal set

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigdelset(
  sigset_t *set,
  int       signo
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

This function deletes the signal specified by @code{signo} from the specified
signal @code{set}.

@subheading NOTES:

The set must be initialized using either @code{sigemptyset} or @code{sigfillset}
before using this function.

@c
@c
@c
@page
@subsection sigfillset - Fill a Signal Set

@findex sigfillset
@cindex  fill a signal set

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigfillset(
  sigset_t *set
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

This function fills the specified signal @code{set} such that all
signals are set.

@c
@c
@c
@page
@subsection sigismember - Is Signal a Member of a Signal Set

@findex sigismember
@cindex  is signal a member of a signal set

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigismember(
  const sigset_t *set,
  int             signo
);
@end example

@subheading STATUS CODES:

The function returns either 1 or 0 if completed successfully, otherwise it
returns -1 and sets @code{errno} to indicate the error. @code{errno} may be set
to:

@table @b

@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

This function returns returns 1 if @code{signo} is a member of @code{set}
and 0 otherwise.

@subheading NOTES:

The set must be initialized using either @code{sigemptyset} or @code{sigfillset}
before using this function.

@c
@c
@c
@page
@subsection sigemptyset - Empty a Signal Set

@findex sigemptyset
@cindex  empty a signal set

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigemptyset(
  sigset_t *set
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b
@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

This function initializes an empty signal set pointed to by @code{set}.

@c
@c
@c
@page
@subsection sigaction - Examine and Change Signal Action

@findex sigaction
@cindex  examine and change signal action

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

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b
@item EINVAL
Invalid argument passed.

@item ENOTSUP
Realtime Signals Extension option not supported.

@end table

@subheading DESCRIPTION:

If the argument act is not a null pointer, it points to a structure specifying
the action to be associated with the specified signal. If the argument oact is
not a null pointer, the action previously associated with the signal is stored
in the location pointed to by the argument oact. If the argument act is a null
pointer, signal handling is unchanged; thus, the call can be used to enquire
about the current handling of a given signal.

The structure @code{sigaction} has the following members:

@table @code

@item void(*)(int) sa_handler
Pointer to a signal-catching function or one of the macros SIG_IGN or SIG_DFL.

@item sigset_t sa_mask
Additional set of signals to be blocked during execution of signal-catching function.

@item int sa_flags
Special flags to affect behavior of signal.

@item void(*)(int, siginfo_t*, void*) sa_sigaction
Alternative pointer to a signal-catching function.

@end table

@code{sa_handler} and @code{sa_sigaction} should never be used at the same time as their storage may overlap.

If the @code{SA_SIGINFO} flag (see below) is set in @code{sa_flags}, the
@code{sa_sigaction} field specifies a signal-catching function, otherwise
@code{sa_handler} specifies the action to be associated with the signal, which
may be a signal-catching function or one of the macros @code{SIG_IGN} or
@code{SIG_DFN}.

The following flags can be set in the @code{sa_flags} field:

@table @code

@c I found no evidence that other flags are used in the source code.
@c I did a fulltext search in cpukit/posix/, maybe I looked in the wrong place?
@item SA_SIGINFO
If not set, the signal-catching function should be declared as @code{void
func(int signo)} and the address of the function should be set in
@code{sa_handler}.  If set, the signal-catching function should be declared as
@code{void func(int signo, siginfo_t* info, void* context)} and the address of
the function should be set in @code{sa_sigaction}.

@end table

The prototype of the @code{siginfo_t} structure is the following:
@example
typedef struct
@{
  int si_signo; /* Signal number */
  int si_code; /* Cause of the signal */
  pid_t si_pid; /* Sending process ID */
  uid_t si_uid; /* Real user ID of sending process */
  void* si_addr; /* Address of faulting instruction */
  int si_status; /* Exit value or signal */
  union sigval
  @{
      int sival_int; /* Integer signal value */
      void* sival_ptr; /* Pointer signal value */
  @} si_value; /* Signal value */
@}
@end example

@subheading NOTES:

The signal number cannot be SIGKILL.

@c
@c
@c
@page
@subsection pthread_kill - Send a Signal to a Thread

@findex pthread_kill
@cindex  send a signal to a thread

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int pthread_kill(
  pthread_t thread,
  int       sig
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b

@item ESRCH
The thread indicated by the parameter thread is invalid.

@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

This functions sends the specified signal @code{sig} to a thread referenced
to by @code{thread}.

If the signal code is @code{0}, arguments are validated and no signal is sent.


@c
@c
@c
@page
@subsection sigprocmask - Examine and Change Process Blocked Signals

@findex sigprocmask
@cindex  examine and change process blocked signals

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigprocmask(
  int             how,
  const sigset_t *set,
  sigset_t       *oset
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b

@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

This function is used to alter the set of currently blocked signals
on a process wide basis. A blocked signal will not be received by the
process. The behavior of this function is dependent on the value of
@code{how} which may be one of the following:

@table @code

@item SIG_BLOCK
The set of blocked signals is set to the union of @code{set} and
those signals currently blocked.

@item SIG_UNBLOCK
The signals specific in @code{set} are removed from the currently
blocked set.

@item SIG_SETMASK
The set of currently blocked signals is set to @code{set}.

@end table

If @code{oset} is not @code{NULL}, then the set of blocked signals prior to
this call is returned in @code{oset}. If @code{set} is @b{NULL}, no change is
done, allowing to examine the set of currently blocked signals.

@subheading NOTES:

It is not an error to unblock a signal which is not blocked.

In the current implementation of RTEMS POSIX API sigprocmask() is technically
mapped to pthread_sigmask().

@c
@c
@c
@page
@subsection pthread_sigmask - Examine and Change Thread Blocked Signals

@findex pthread_sigmask
@cindex  examine and change thread blocked signals

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int pthread_sigmask(
  int             how,
  const sigset_t *set,
  sigset_t       *oset
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b

@item EINVAL
Invalid argument passed.

@end table

@subheading DESCRIPTION:

This function is used to alter the set of currently blocked signals
for the calling thread. A blocked signal will not be received by the
process. The behavior of this function is dependent on the value of
@code{how} which may be one of the following:

@table @code
@item SIG_BLOCK
The set of blocked signals is set to the union of @code{set} and
those signals currently blocked.

@item SIG_UNBLOCK
The signals specific in @code{set} are removed from the currently
blocked set.

@item SIG_SETMASK
The set of currently blocked signals is set to @code{set}.

@end table

If @code{oset} is not @code{NULL}, then the set of blocked signals prior to
this call is returned in @code{oset}. If @code{set} is @b{NULL}, no change is
done, allowing to examine the set of currently blocked signals.

@subheading NOTES:

It is not an error to unblock a signal which is not blocked.


@c
@c
@c
@page
@subsection kill - Send a Signal to a Process

@findex kill
@cindex  send a signal to a process

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

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b
@item EINVAL
Invalid argument passed.

@item EPERM
Process does not have permission to send the signal to any receiving process.

@item ESRCH
The process indicated by the parameter pid is invalid.

@end table

@subheading DESCRIPTION:

This function sends the signal @code{sig} to the process @code{pid}.

@subheading NOTES:

Since RTEMS is a single-process system, a signal can only be sent to the calling
process (i.e. the current node).

@c
@c
@c
@page
@subsection sigpending - Examine Pending Signals

@findex sigpending
@cindex  examine pending signals

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigpending(
  const sigset_t *set
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b

@item EFAULT
Invalid address for set.

@end table

@subheading DESCRIPTION:

This function allows the caller to examine the set of currently pending
signals. A pending signal is one which has been raised but is currently
blocked. The set of pending signals is returned in @code{set}.

@c
@c
@c
@page
@subsection sigsuspend - Wait for a Signal

@findex sigsuspend
@cindex  wait for a signal

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigsuspend(
  const sigset_t *sigmask
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b

@item EINTR
Signal interrupted this function.

@end table

@subheading DESCRIPTION:

This function temporarily replaces the signal mask for the process
with that specified by @code{sigmask} and blocks the calling thread
until a signal is raised.

@c
@c
@c
@page
@subsection pause - Suspend Process Execution

@findex pause
@cindex  suspend process execution

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int pause( void );
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b

@item EINTR
Signal interrupted this function.

@end table

@subheading DESCRIPTION:

This function causes the calling thread to be blocked until an
unblocked signal is received.

@c
@c
@c
@page
@subsection sigwait - Synchronously Accept a Signal

@findex sigwait
@cindex  synchronously accept a signal

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigwait(
  const sigset_t *set,
  int            *sig
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b

@item EINVAL
Invalid argument passed.

@item EINTR
Signal interrupted this function.

@end table

@subheading DESCRIPTION:

This function selects a pending signal based on the set specified in
@code{set}, atomically clears it from the set of pending signals, and
returns the signal number for that signal in @code{sig}.

@c
@c
@c
@page
@subsection sigwaitinfo - Synchronously Accept a Signal

@findex sigwaitinfo
@cindex  synchronously accept a signal

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigwaitinfo(
  const sigset_t *set,
  siginfo_t      *info
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b

@item EINTR
Signal interrupted this function.

@end table

@subheading DESCRIPTION:

This function selects a pending signal based on the set specified in
@code{set}, atomically clears it from the set of pending signals, and
returns information about that signal in @code{info}.

The prototype of the @code{siginfo_t} structure is the following:
@example
typedef struct
@{
  int si_signo; /* Signal number */
  int si_code; /* Cause of the signal */
  pid_t si_pid; /* Sending process ID */
  uid_t si_uid; /* Real user ID of sending process */
  void* si_addr; /* Address of faulting instruction */
  int si_status; /* Exit value or signal */
  union sigval
  @{
      int sival_int; /* Integer signal value */
      void* sival_ptr; /* Pointer signal value */
  @} si_value; /* Signal value */
@}
@end example

@c
@c
@c
@page
@subsection sigtimedwait - Synchronously Accept a Signal with Timeout

@findex sigtimedwait
@cindex  synchronously accept a signal with timeout

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigtimedwait(
  const sigset_t        *set,
  siginfo_t             *info,
  const struct timespec *timeout
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b

@item EAGAIN
Timed out while waiting for the specified signal set.

@item EINVAL
Nanoseconds field of the timeout argument is invalid.

@item EINTR
Signal interrupted this function.

@end table

@subheading DESCRIPTION:

This function selects a pending signal based on the set specified in
@code{set}, atomically clears it from the set of pending signals, and
returns information about that signal in @code{info}. The calling thread
will block up to @code{timeout} waiting for the signal to arrive.

The @code{timespec} structure is defined as follows:
@example
struct timespec
@{
  time_t tv_sec; /* Seconds */
  long tv_nsec; /* Nanoseconds */
@}
@end example

@subheading NOTES:

If @code{timeout} is NULL, then the calling thread will wait forever for
the specified signal set.

@c
@c
@c
@page
@subsection sigqueue - Queue a Signal to a Process

@findex sigqueue
@cindex  queue a signal to a process

@subheading CALLING SEQUENCE:

@example
#include <signal.h>

int sigqueue(
  pid_t              pid,
  int                signo,
  const union sigval value
);
@end example

@subheading STATUS CODES:

The function returns 0 on success, otherwise it returns -1 and sets
@code{errno} to indicate the error. @code{errno} may be set to:

@table @b

@item EAGAIN
No resources available to queue the signal. The process has already
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

This function sends the signal specified by @code{signo} to the
process @code{pid}

The @code{sigval} union is specified as:
@example
union sigval
@{
  int sival_int; /* Integer signal value */
  void* sival_ptr; /* Pointer signal value */
@}
@end example

@subheading NOTES:

Since RTEMS is a single-process system, a signal can only be sent to the calling
process (i.e. the current node).

@c
@c
@c
@page
@subsection alarm - Schedule Alarm

@findex alarm
@cindex  schedule alarm

@subheading CALLING SEQUENCE:

@example
#include <unistd.h>

unsigned int alarm(
  unsigned int seconds
);
@end example

@subheading STATUS CODES:

This call always succeeds.

If there was a previous @code{alarm()} request with time remaining,
then this routine returns the number of seconds until that outstanding
alarm would have fired. If no previous @code{alarm()} request was
outstanding, then zero is returned.

@subheading DESCRIPTION:

The @code{alarm()} service causes the @code{SIGALRM} signal to
be generated after the number of seconds specified by
@code{seconds} has elapsed.

@subheading NOTES:

Alarm requests do not queue.  If @code{alarm} is called while
a previous request is outstanding, the call will result in
rescheduling the time at which the @code{SIGALRM} signal
will be generated.

If the notification signal, @code{SIGALRM}, is not caught or ignored, the
calling process is terminated.

@c
@c
@c
@page
@subsection ualarm - Schedule Alarm in Microseconds

@findex alarm
@findex microseonds alarm
@findex usecs alarm
@cindex schedule alarm in microseonds

@subheading CALLING SEQUENCE:

@example
#include <unistd.h>

useconds_t ualarm(
  useconds_t useconds,
  useconds_t interval
);
@end example

@subheading STATUS CODES:

This call always succeeds.

If there was a previous @code{ualarm()} request with time remaining,
then this routine returns the number of seconds until that outstanding
alarm would have fired. If no previous @code{alarm()} request was
outstanding, then zero is returned.

@subheading DESCRIPTION:

The @code{ualarm()} service causes the @code{SIGALRM} signal to
be generated after the number of microseconds specified by
@code{useconds} has elapsed.

When @code{interval} is non-zero, repeated timeout notification occurs
with a period in microseconds specified by @code{interval}.

@subheading NOTES:

Alarm requests do not queue.  If @code{alarm} is called while
a previous request is outstanding, the call will result in
rescheduling the time at which the @code{SIGALRM} signal
will be generated.

If the notification signal, @code{SIGALRM}, is not caught or ignored, the
calling process is terminated.

