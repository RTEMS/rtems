@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Process Primitives

@section Process Creation and Execution

@subsection Process Creation

@example
fork(), Function, Unimplementable
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
@end example

@subsection Terminate a Process

@example
_exit(), Function, Unimplemented
@end example

@section Signals

@subsection Signal Concepts

@example
sigset_t, Type,
struct sigevent, Type,
union sigval, Type,
siginfo_t, Type,
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
sigaction, Type, 
@end example

@subsection Examine and Change Blocked Signals

@example
pthread_sigmask(), Function, Implemented
sigprocmask(), Function, Implemented
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
