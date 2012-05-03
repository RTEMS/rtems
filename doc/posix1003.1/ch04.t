@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.

@chapter Process Environment

@section Process Identification

@subsection Get Process and Parent Process IDs

@example
getpid(), Function, Implemented, SUSP Functionality
getppid(), Function, Implemented, SUSP Functionality
@end example

@section User Identification

@subsection Get Real User Effective User Real Group and Effective Group IDs

@example
getuid(), Function, Implemented, SUSP Functionality
geteuid(), Function, Implemented, SUSP Functionality
getgid(), Function, Implemented, SUSP Functionality
getegid(), Function, Implemented, SUSP Functionality
@end example

@subsection Set User and Group IDs

@example
setuid(), Function, Implemented, SUSP Functionality
setgid(), Function, Implemented, SUSP Functionality
@end example

@subsection Get Supplementary Group IDs

@example
getgroups(), Function, Implemented, SUSP Functionality
@end example

@subsection Get User Name

@example
getlogin(), Function, Implemented, SUSP Functionality
getlogin_r(), Function, Implemented, SUSP Functionality
@end example

@section Process Groups

@subsection Get Process Group ID

@example
getpgrp(), Function, Implemented, SUSP Functionality
@end example

@subsection Create Session and Set Process Group ID

@example
setsid(), Function, Implemented, SUSP Functionality
@end example

@subsection Set Process Group ID for Job Control

@example
setpgid(), Function, Dummy Implementation
@end example

@section System Identification

@subsection Get System Name

@example
struct utsname, Type, Implemented
uname(), Function, Implemented
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
ctermid(), Function, Implemented
@end example

@subsection Determine Terminal Device Name

@example
ttyname(), Function, Implemented, untested
ttyname_r(), Function, Implemented, untested
isatty(), Function, Implemented
@end example

@section Configurable System Variables

@subsection Get Configurable System Variables

@example
sysconf(), Function, Dummy Implementation
_SC_AIO_LISTIO_MAX, Constant, Implemented
_SC_AIO_MAX, Constant, Implemented
_SC_AIO_PRIO_DELTA_MAX, Constant, Implemented
_SC_ARG_MAX, Constant, Implemented
_SC_CHILD_MAX, Constant, Implemented
_SC_CLK_TCK, Constant, Implemented
CLK_TCK, Constant, Implemented
_SC_DELAYTIMER_MAX, Constant, Implemented
_SC_GETGR_R_SIZE_MAX, Constant, Implemented
_SC_GETPW_R_SIZE_MAX, Constant, Implemented
_SC_LOGIN_NAME_MAX, Constant, Implemented
_SC_MQ_OPEN_MAX, Constant, Implemented
_SC_MQ_PRIO_MAX, Constant, Implemented
_SC_NGROUPS_MAX, Constant, Implemented
_SC_OPEN_MAX, Constant, Implemented
_SC_PAGESIZE, Constant, Implemented
_SC_RTSIG_MAX, Constant, Implemented
_SC_SEM_NSEMS_MAX, Constant, Implemented
_SC_SEM_VALUE_MAX, Constant, Implemented
_SC_SIGQUEUE_MAX, Constant, Implemented
_SC_STREAM_MAX, Constant, Implemented
_SC_THREAD_DESTRUCTOR_ITERATIONS, Constant, Implemented
_SC_THREAD_KEYS_MAX, Constant, Implemented
_SC_THREAD_STACK_MIN, Constant, Implemented
_SC_THREAD_THREADS_MAX, Constant, Implemented
_SC_TIMER_MAX, Constant, Implemented
_SC_TTY_NAME_MAX, Constant, Implemented
_SC_TZNAME_MAX, Constant, Implemented
_SC_ASYNCHRONOUS_IO, Constant, Implemented
_SC_FSYNC, Constant, Implemented
_SC_JOB_CONROL, Constant, Implemented
_SC_MAPPED_FILES, Constant, Implemented
_SC_MEMLOCK, Constant, Implemented
_SC_MEMLOCK_RANGE, Constant, Implemented
_SC_MEMORY_PROTECTION, Constant, Implemented
_SC_MESSAGE_PASSING, Constant, Implemented
_SC_PRIORITIZED_IO, Constant, Implemented
_SC_PRIORITY_SCHEDULING, Constant, Unimplemented
_SC_REALTIME_SIGNALS, Constant, Implemented
_SC_SAVED_IDS, Constant, Implemented
_SC_SEMAPHORES, Constant, Implemented
_SC_SHARED_MEMORY_OBJECTS, Constant, Implemented
_SC_SYNCHRONIZED_IO, Constant, Implemented
_SC_TIMERS, Constant, Implemented
_SC_THREADS, Constant, Implemented
_SC_THREAD_ATTR_STACKADDR, Constant, Implemented
_SC_THREAD_ATTR_STACKSIZE, Constant, Implemented
_SC_THREAD_PRIORITY_SCHEDULING, Constant, Implemented
_SC_THREAD_PRIO_INHERIT, Constant, Implemented
_SC_THREAD_PRIO_PROTECT, Constant, Unimplemented
_SC_THREAD_PROCESS_SHARED, Constant, Implemented
_SC_THREAD_SAFE_FUNCTIONS, Constant, Implemented
_SC_VERSION, Constant, Implemented
@end example
