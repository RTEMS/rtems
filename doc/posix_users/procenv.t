@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Process Environment Manager

@section Introduction

The 
process environment manager is ...

The directives provided by the process environment manager are:

@itemize @bullet
@item @code{getpid} - 
@item @code{getppid} - 
@item @code{getuid} - 
@item @code{geteuid} - 
@item @code{getgid} - 
@item @code{getegid} - 
@item @code{setuid} - 
@item @code{setgid} - 
@item @code{getgroups} - 
@item @code{getlogin} - 
@item @code{getlogin_r} - 
@item @code{getpgrp} - 
@item @code{setsid} - 
@item @code{setpgid} - 
@item @code{uname} - 
@item @code{times} - Get process times
@item @code{getenv} - 
@item @code{ctermid} - 
@item @code{ttyname} - 
@item @code{ttyname_r} - 
@item @code{isatty} - 
@item @code{sysconf} - 
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
@subsection getpid - 

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
@subsection getppid - 

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
@subsection getuid - 

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
@subsection geteuid - 

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
@subsection getgid - 

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
@subsection getegid - 

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
@subsection setuid - 

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
@subsection setgid - 

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
@subsection getgroups - 

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
@subsection getlogin - 

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
@subsection getlogin_r - 

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
@subsection getpgrp - 

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
@subsection setsid - 

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
@subsection setpgid - 

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
@subsection uname - 

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
@subsection getenv - 

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
@subsection ctermid - 

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
@subsection ttyname - 

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
@subsection ttyname_r - 

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
@subsection isatty - 

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
@subsection sysconf - 

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

