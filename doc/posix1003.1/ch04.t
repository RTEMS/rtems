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
getuid(), Function
geteuid(), Function
getgid(), Function
getegid(), Function
@end example

@subsection Set User and Group IDs

@example
setuid(), Function
setgid(), Function
@end example

@subsection Get Supplementary Group IDs

@example
getgroups(), Function
@end example

@subsection Get User Name

@example
getlogin(), Function
getlogin_r(), Function
@end example

@section Process Groups

@subsection Get Process Group ID

@example
getpgrp(), Function
@end example

@subsection Create Session and Set Process Group ID

@example
setsid(), Function
@end example

@subsection Set Process Group ID for Job Control

@example
setpgid(), Function
@end example

@section System Identification

@subsection Get System Name

@example
uname(), Function
@end example

@section Time

@subsection Get System Time

@example
time(), Function
@end example

@subsection Get Process Times

@example
times(), Function
@end example

@section Environment Variables

@subsection Environment Access

@example
getenv(), Function
@end example

@section Terminal Identification

@subsection Generate Terminal Pathname

@example
ctermid(), Function
@end example

@subsection Determine Terminal Device Name

@example
ttyname(), Function
ttyname_r(), Function
isatty(), Function
@end example

@section Configurable System Variables

@subsection Get Configurable System Variables

@example
sysconf(), Function
@end example
