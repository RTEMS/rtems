@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.
@c
@c $Id$
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

@c
@c
@c
@page
@subsection getpid - Get Process ID

@findex getpid
@cindex  get process id

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

@c
@c
@c
@page
@subsection getppid - Get Parent Process ID

@findex getppid
@cindex  get parent process id

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

@c
@c
@c
@page
@subsection getuid - Get User ID

@findex getuid
@cindex  get user id

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

@c
@c
@c
@page
@subsection geteuid - Get Effective User ID

@findex geteuid
@cindex  get effective user id

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

@c
@c
@c
@page
@subsection getgid - Get Real Group ID

@findex getgid
@cindex  get real group id

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

@c
@c
@c
@page
@subsection getegid - Get Effective Group ID

@findex getegid
@cindex  get effective group id

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

@c
@c
@c
@page
@subsection setuid - Set User ID

@findex setuid
@cindex  set user id

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

@c
@c
@c
@page
@subsection setgid - Set Group ID

@findex setgid
@cindex  set group id

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

@c
@c
@c
@page
@subsection getgroups - Get Supplementary Group IDs

@findex getgroups
@cindex  get supplementary group ids

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

@c
@c
@c
@page
@subsection getlogin - Get User Name

@findex getlogin
@cindex  get user name

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

@c
@c
@c
@page
@subsection getlogin_r - Reentrant Get User Name

@findex getlogin_r
@cindex  reentrant get user name

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

@c
@c
@c
@page
@subsection getpgrp - Get Process Group ID

@findex getpgrp
@cindex  get process group id

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

@c
@c
@c
@page
@subsection setsid - Create Session and Set Process Group ID

@findex setsid
@cindex  create session and set process group id

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

@c
@c
@c
@page
@subsection setpgid - Set Process Group ID for Job Control

@findex setpgid
@cindex  set process group id for job control

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

@c
@c
@c
@page
@subsection uname - Get System Name

@findex uname
@cindex  get system name

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

@c
@c
@c
@page
@subsection times - Get process times

@findex times
@cindex  get process times

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

@c
@c
@c
@page
@subsection getenv - Get Environment Variables

@findex getenv
@cindex  get environment variables

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

@c
@c
@c
@page
@subsection setenv - Set Environment Variables

@findex setenv
@cindex  set environment variables

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

@c
@c
@c
@page
@subsection ctermid - Generate Terminal Pathname

@findex ctermid
@cindex  generate terminal pathname

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

@c
@c
@c
@page
@subsection ttyname - Determine Terminal Device Name

@findex ttyname
@cindex  determine terminal device name

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

@c
@c
@c
@page
@subsection ttyname_r - Reentrant Determine Terminal Device Name

@findex ttyname_r
@cindex  reentrant determine terminal device name

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

@c
@c
@c
@page
@subsection isatty - Determine if File Descriptor is Terminal

@findex isatty
@cindex  determine if file descriptor is terminal

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

@c
@c
@c
@page
@subsection sysconf - Get Configurable System Variables

@findex sysconf
@cindex  get configurable system variables

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

