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
getpid(), Function
getppid(), Function
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
struct utsname, Type, 
uname(), Function, Untested Implementation
@end example

@section Time

@subsection Get System Time

@example
time(), Function, Implemented
@end example

@subsection Get Process Times

@example
struct tms, Type,
times(), Function, Implemented
@end example

NOTE: times() always returns 0 for tms_stime, tms_cutime, and
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
ttyname_r(), Unimplemented
isatty(), Function, Dummy Implementation
@end example

@section Configurable System Variables

@subsection Get Configurable System Variables

@example
sysconf(), Function, Not Implemented
@end example
