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
getpid()
getppid()
@end example

@section User Identification

@subsection Get Real User Effective User Real Group and Effective Group IDs

@example
getuid()
geteuid()
getgid()
getegid()
@end example

@subsection Set User and Group IDs

@example
setuid()
setgid()
@end example

@subsection Get Supplementary Group IDs

@example
getgroups()
@end example

@subsection Get User Name

@example
getlogin()
getlogin_r()
@end example

@section Process Groups

@subsection Get Process Group ID

@example
getpgrp()
@end example

@subsection Create Session and Set Process Group ID

@example
setsid()
@end example

@subsection Set Process Group ID for Job Control

@example
setpgid()
@end example

@section System Identification

@subsection Get System Name

@example
uname()
@end example

@section Time

@subsection Get System Time

@example
time()
@end example

@subsection Get Process Times

@example
times()
@end example

@section Environment Variables

@subsection Environment Access

@example
getenv()
@end example

@section Terminal Identification

@subsection Generate Terminal Pathname

@example
ctermid()
@end example

@subsection Determine Terminal Device Name

@example
ttyname()
ttyname_r()
isatty()
@end example

@section Configurable System Variables

@subsection Get Configurable System Variables

@example
sysconf()
@end example
