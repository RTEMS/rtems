@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Process Environment Manager

@section Introduction

The process environment manager is ...

The directives provided by the process environment manager are:

@itemize @bullet
@item @code{getpid} - Get Process ID
@item @code{getppid} - Get Parent Process ID
@item @code{getuid} - Get User ID
@item @code{geteuid} - Get Effective User ID
@item @code{getgid} - Get Real Group ID
@item @code{getegid} - Get Effective Group ID
@item @code{setuid} - Set User ID
@item @code{setgid} - Set Group ID
@item @code{getgroups} - Get Supplementary Group IDs
@item @code{getlogin} - Get User Name
@item @code{getlogin_r} - Reentrant Get User Name
@item @code{getpgrp} - Get Process Group ID
@item @code{setsid} - Create Session and Set Process Group ID
@item @code{setpgid} - Set Process Group ID for Job Control
@item @code{uname} - Get System Name
@item @code{times} - Get Process Times
@item @code{getenv} - Get Environment Variables
@item @code{setenv} - Set Environment Variables
@item @code{ctermid} - Generate Terminal Pathname
@item @code{ttyname} - Determine Terminal Device Name
@item @code{ttyname_r} - Reentrant Determine Terminal Device Name
@item @code{isatty} - Determine if File Descriptor is Terminal
@item @code{sysconf} - Get Configurable System Variables
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the process environment manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection getpid - Get Process ID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getpid(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection getppid - Get Parent Process ID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getppid(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection getuid - Get User ID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getuid(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection geteuid - Get Effective User ID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int geteuid(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection getgid - Get Real Group ID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getgid(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection getegid - Get Effective Group ID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getegid(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection setuid - Set User ID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int setuid(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection setgid - Set Group ID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int setgid(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection getgroups - Get Supplementary Group IDs

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getgroups(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection getlogin - Get User Name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getlogin(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection getlogin_r - Reentrant Get User Name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getlogin_r(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection getpgrp - Get Process Group ID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getpgrp(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection setsid - Create Session and Set Process Group ID

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int setsid(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection setpgid - Set Process Group ID for Job Control

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int setpgid(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection uname - Get System Name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int uname(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection times - Get process times

@subheading CALLING SEQUENCE:

@example
#include <sys/time.h>

clock_t times(
  struct tms *buf
);
@end example

@subheading STATUS CODES:

This routine returns the process times

@subheading DESCRIPTION:

@code{times} stores the current process times in @code{buf}.

@code{struct tms} is as defined in @code{/usr/include/sys/times.h}

@code{times} returns the number of clock ticks that have elapsed
since the systm has been up.

@subheading NOTES:

NONE

@page
@subsection getenv - Get Environment Variables

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getenv(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection setenv - Set Environment Variables

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int setenv(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection ctermid - Generate Terminal Pathname

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ctermid(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection ttyname - Determine Terminal Device Name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ttyname(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection ttyname_r - Reentrant Determine Terminal Device Name

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ttyname_r(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection isatty - Determine if File Descriptor is Terminal

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int isatty(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

@page
@subsection sysconf - Get Configurable System Variables

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sysconf(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@table @b
@item E
The

@end table

@subheading DESCRIPTION:

@subheading NOTES:

