@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Process Primitives

@section Process Creation and Execution

@subsection Process Creation

@example
fork(), Function, Unimplementable, Requires Processes
@end example


@subsection Execute a File

@example
execl(), Function, Unimplementable, Requires Processes
execv(), Function, Unimplementable, Requires Processes
execle(), Function, Unimplementable, Requires Processes
execve(), Function, Unimplementable, Requires Processes
execlp(), Function, Unimplementable, Requires Processes
execvp(), Function, Unimplementable, Requires Processes
@end example

@subsection Register Fork Handlers

@example
pthread_atfork(), Function, Unimplementable, Requires Processes
@end example

@section Process Termination

@subsection Wait for Process Termination

@example
wait(), Function, Unimplementable, Requires Processes
waitpid(), Function, Unimplementable, Requires Processes
WNOHANG, Constant, Unimplementable, Requires Processes
WUNTRACED, Constant, Unimplementable, Requires Processes
WIFEXITED(), Function, Unimplementable, Requires Processes
WEXITSTATUS(), Function, Unimplementable, Requires Processes
WIFSIGNALED(), Function, Unimplementable, Requires Processes
WTERMSIG(), Function, Unimplementable, Requires Processes
WIFSTOPPED(), Function, Unimplementable, Requires Processes
WSTOPSIG(), Function, Unimplementable, Requires Processes
@end example

@subsection Terminate a Process

@example
_exit(), Function, Implemented
@end example

@section Signals

@subsection Signal Concepts

@subsubsection Signal Names

@example
sigset_t, Type, Implemented
SIG_DFL, Constant, Implemented
SIG_IGN, Constant, Implemented
SIG_ERR, Constant, Implemented
SIGABRT, Constant, Implemented
SIGALRM, Constant, Implemented
SIGFPE, Constant, Implemented
SIGHUP, Constant, Implemented
SIGILL, Constant, Implemented
SIGINT, Constant, Implemented
SIGKILL, Constant, Implemented
SIGPIPE, Constant, Implemented
SIGQUIT, Constant, Implemented
SIGSEGV, Constant, Implemented
SIGTERM, Constant, Implemented
SIGUSR1, Constant, Implemented
SIGUSR2, Constant, Implemented
SIGCHLD, Constant, Unimplemented
SIGCONT, Constant, Unimplemented
SIGSTOP, Constant, Unimplemented
SIGTSTP, Constant, Unimplemented
SIGTTIN, Constant, Unimplemented
SIGTTOU, Constant, Unimplemented
SIGBUS, Constant, Implemented
SIGRTMIN, Constant, Implemented
SIGRTMAX, Constant, Implemented
@end example

NOTE: SIG_ERR is technically an extension to the C Library which is
not documented anywhere else according to the index.
@subsubsection Signal Generation and Delivery

@example
struct sigevent, Type, Implemented
union sigval, Type, Implemented
SIGEV_NONE, Constant, Implemented
SIGEV_SIGNAL, Constant, Implemented
SIGEV_THREAD, Constant, Implemented
@end example

@subsubsection Signal Actions

@example
siginfo_t, Type, Implemented
SI_USER, Constant, Implemented
SI_QUEUE, Constant, Implemented
SI_TIMER, Constant, Implemented
SI_ASYNCIO, Constant, Implemented
SI_MESGQ, Constant, Implemented
@end example

@subsection Send a Signal to a Process

@example
kill(), Function, Implemented
@end example

@subsection Manipulate Signal Sets

@example
sigemptyset(), Function, Implemented
sigfillset(), Function, Implemented
sigaddset(), Function, Implemented
sigdelset(), Function, Implemented
sigismember(), Function, Implemented
@end example

@subsection Examine and Change Signal Action

@example
sigaction(), Function, Implemented
sigaction, Type, Implemented
SA_NOCLDSTOP, Constant, Implemented
SA_SIGINFO, Constant, Implemented
@end example

@subsection Examine and Change Blocked Signals

@example
pthread_sigmask(), Function, Implemented
sigprocmask(), Function, Implemented
SIG_BLOCK, Constant, Implemented
SIG_UNBLOCK, Constant, Implemented
SIG_SETMASK, Constant, Implemented
@end example

@subsection Examine Pending Signals

@example
sigpending(), Function, Implemented
@end example

@subsection Wait for a Signal

@example
sigsuspend(), Function, Implemented
@end example

@subsection Synchronously Accept a Signal

@example
sigwait(), Function, Implemented
sigwaitinfo(), Function, Implemented
sigtimedwait(), Function, Implemented
@end example

@subsection Queue a Signal to a Process

@example
sigqueue(), Function, Implemented
@end example

@subsection Send a Signal to a Thread

@example
pthread_kill(), Function, Implemented
@end example

@section Timer Operations

@subsection Schedule Alarm

@example
alarm(), Function, Implemented
@end example

@subsection Suspend Process Execution

@example
pause(), Function, Implemented
@end example

@subsection Delay Process Execution

@example
sleep(), Function, Implemented
@end example
