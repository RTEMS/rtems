@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Files and Directories Manager

@section Introduction

The 
files and directories manager is ...

The directives provided by the files and directories manager are:

@itemize @bullet
@item @code{opendir} - 
@item @code{readdir} - 
@item @code{readdir_r} - 
@item @code{rewinddir} - 
@item @code{closedir} - 
@item @code{chdir} - 
@item @code{getcwd} - 
@item @code{open} - 
@item @code{creat} - 
@item @code{umask} - 
@item @code{link} - 
@item @code{mkdir} - 
@item @code{mkfifo} - 
@item @code{unlink} - 
@item @code{rmdir} - 
@item @code{rename} - 
@item @code{stat} - 
@item @code{fstat} - 
@item @code{access} - 
@item @code{chmod} - 
@item @code{fchmod} - 
@item @code{chown} - 
@item @code{utime} - 
@item @code{ftrunctate} - 
@item @code{pathconf} - 
@item @code{fpathconf} - 
@end itemize

@section Background

@section Operations

@section Directives

This section details the files and directories manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection opendir - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int opendir(
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
@subsection readdir - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int readdir(
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
@subsection readdir_r - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int readdir_r(
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
@subsection rewinddir - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int rewinddir(
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
@subsection closedir - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int closedir(
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
@subsection chdir - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int chdir(
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
@subsection getcwd - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int getcwd(
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
@subsection open - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int open(
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
@subsection creat - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int creat(
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
@subsection umask - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int umask(
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
@subsection link - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int link(
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
@subsection mkdir - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int mkdir(
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
@subsection mkfifo - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int mkfifo(
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
@subsection unlink - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int unlink(
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
@subsection rmdir - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int rmdir(
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
@subsection rename - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int rename(
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
@subsection stat - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int stat(
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
@subsection fstat - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fstat(
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
@subsection access - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int access(
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
@subsection chmod - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int chmod(
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
@subsection fchmod - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fchmod(
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
@subsection chown - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int chown(
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
@subsection utime - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int utime(
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
@subsection ftrunctate - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ftrunctate(
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
@subsection pathconf - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pathconf(
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
@subsection fpathconf - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fpathconf(
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

