@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Process Environment

@section Process Identification

@subsection Get Process and Parent Process IDs

@example
getpid(), Function, Dummy Implementation
getppid(), Function, Dummy Implementation
@end example

@section User Identification

@subsection Get Real User Effective User Real Group and Effective Group IDs

@example
getuid(), Function, Dummy Implementation
geteuid(), Function, Dummy Implementation
getgid(), Function, Dummy Implementation
getegid(), Function, Dummy Implementation
@end example

@subsection Set User and Group IDs

@example
setuid(), Function, Dummy Implementation
setgid(), Function, Dummy Implementation
@end example

@subsection Get Supplementary Group IDs

@example
getgroups(), Function, Dummy Implementation
@end example

@subsection Get User Name

@example
getlogin(), Function, Dummy Implementation
getlogin_r(), Function, Dummy Implementation
@end example

@section Process Groups

@subsection Get Process Group ID

@example
getpgrp(), Function, Dummy Implementation
@end example

@subsection Create Session and Set Process Group ID

@example
setsid(), Function, Dummy Implementation
@end example

@subsection Set Process Group ID for Job Control

@example
setpgid(), Function, Dummy Implementation
@end example

@section System Identification

@subsection Get System Name

@example
struct utsname, Type, Untested Implementation
uname(), Function, Untested Implementation
@end example

@section Time

@subsection Get System Time

@example
time(), Function, Implemented
@end example

@subsection Get Process Times

@example
struct tms, Type, Implemented
times(), Function, Implemented
@end example

NOTE: @code{times} always returns 0 for tms_stime, tms_cutime, and
tms_cstime fields of the @code{struct tms} returned.

@section Environment Variables

@subsection Environment Access

@example
getenv(), Function, Implemented
@end example

@section Terminal Identification

@subsection Generate Terminal Pathname

@example
ctermid(), Function, Unimplemented
@end example

@subsection Determine Terminal Device Name

@example
ttyname(), Function, Untested Implementation, assumes directory services
ttyname_r(), Function, Unimplemented
isatty(), Function, Partial Implementation
@end example

@section Configurable System Variables

@subsection Get Configurable System Variables

@example
sysconf(), Function, Not Implemented
_SC_AIO_LISTIO_MAX, Constant, 
_SC_AIO_MAX, Constant, 
_SC_AIO_PRIO_DELTA_MAX, Constant, 
_SC_ARG_MAX, Constant, 
_SC_CHILD_MAX, Constant, 
_SC_CLK_TCK, Constant, 
_SC_DELAYTIMER_MAX, Constant, 
_SC_GETGR_R_SIZE_MAX, Constant, 
_SC_GETPW_R_SIZE_MAX, Constant, 
_SC_LOGIN_NAME_MAX, Constant, 
_SC_MQ_OPEN_MAX, Constant, 
_SC_MQ_PRIO_MAX, Constant, 
_SC_NGROUPS_MAX, Constant, 
_SC_OPEN_MAX, Constant, 
_SC_PAGESIZE, Constant, 
_SC_RTSIG_MAX, Constant, 
_SC_SEM_NSEMS_MAX, Constant, 
_SC_SEM_VALUE_MAX, Constant, 
_SC_SIGQUEUE_MAX, Constant, 
_SC_STREAM_MAX, Constant, 
_SC_THREAD_DESTRUCTOR_ITERATIONS, Constant, 
_SC_THREAD_KEYS_MAX, Constant, 
_SC_THREAD_STACK_MIN, Constant, 
_SC_THREAD_THREADS_MAX, Constant, 
_SC_TIMER_MAX, Constant, 
_SC_TTY_NAME_MAX, Constant, 
_SC_TZNAME_MAX, Constant, 
_SC_ASYNCHRONOUS_IO, Constant, 
_SC_FSYNC, Constant, 
_SC_JOB_CONROL, Constant, 
_SC_MAPPED_FILES, Constant, 
_SC_MEMLOCK, Constant, 
_SC_MEMLOCK_RANGE, Constant, 
_SC_MEMORY_PROTECTION, Constant, 
_SC_MESSAGE_PASSING, Constant, 
_SC_PRIORITIZED_IO, Constant, 
_SC_PRIORITY_SCHEDULING, Constant, 
_SC_REALTIME_SIGNALS, Constant, 
_SC_SAVED_IDS, Constant, 
_SC_SEMAPHORES, Constant, 
_SC_SHARED_MEMORY_OBJECTS, Constant, 
_SC_SYNCHRONIZED_IO, Constant, 
_SC_TIMERS, Constant, 
_SC_THREADS, Constant, 
_SC_THREAD_ATTR_STACKADDR, Constant, 
_SC_THREAD_ATTR_STACKSIZE, Constant, 
_SC_THREAD_PRIORITY_SCHEDULING, Constant, 
_SC_THREAD_PRIO_INHERIT, Constant, 
_SC_THREAD_PRIO_PROTECT, Constant, 
_SC_THREAD_PROCESS_SHARED, Constant, 
_SC_THREAD_SAFE_FUNCTIONS, Constant, 
_SC_VERSION, Constant, 
_CLK_TCK, Constant, 
@end example
