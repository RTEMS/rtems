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
fork(), Unimplementable
@end example


@subsection Execute a File

@example
execl(), Unimplementable
execv(), Unimplementable
execle(), Unimplementable
execve(), Unimplementable
execlp(), Unimplementable
execvp(), Unimplementable
@end example

@subsection Register Fork Handlers

@example
pthread_atfork(), Implemented
@end example

@section Process Termination

@subsection Wait for Process Termination

@example
wait(), Implemented
waitpid(), Implemented
@end example

@subsection Terminate a Process

@example
_exit(), Implemented
@end example

@section Signals

@subsection Signal Concepts

@subsection Send a Signal to a Process

@example
kill(), Implemented
@end example

@subsection Manipulate Signal Sets

@example
sigemptyset(), Implemented
sigfillset(), Implemented
sigaddset(), Implemented
sigdelset(), Implemented
sigismember(), Implemented
@end example

@subsection Examine and Change Signal Action

@example
sigaction(), Implemented
@end example

@subsection Examine and Change Blocked Signals

@example
pthread_sigmask(), Implemented
sigprocmask(), Implemented
@end example

@subsection Examine Pending Signals

@example
sigpending(), Implemented
@end example

@subsection Wait for a Signal

@example
sigsuspend(), Implemented
@end example

@subsection Synchronously Accept a Signal

@example
sigwait(), Implemented
sigwaitinfo(), Implemented
sigtimedwait(), Implemented
@end example

@subsection Queue a Signal to a Process

@example
sigqueue(), Implemented
@end example

@subsection Send a Signal to a Thread

@example
pthread_kill(), Implemented
@end example

@section Timer Operations

@subsection Schedule Alarm

@example
alarm(), Implemented
@end example

@subsection Suspend Process Execution

@example
pause(), Implemented
@end example

@subsection Delay Process Execution

@example
sleep(), Implemented
@end example
