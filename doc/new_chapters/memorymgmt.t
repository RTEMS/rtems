@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Memory Management Manager

@section Introduction

The 
memory management manager is ...

The directives provided by the memory management manager are:

@itemize @bullet
@item @code{mlockall} - 
@item @code{munlockall} - 
@item @code{mlock} - 
@item @code{munlock} - 
@item @code{mmap} - 
@item @code{munmap} - 
@item @code{mprotect} - 
@item @code{msync} - 
@item @code{shm_open} - 
@item @code{shm_unlink} - 
@end itemize

@section Background

@section Operations

@section Directives

This section details the memory management manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection mlockall - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int mlockall(
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
@subsection munlockall - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int munlockall(
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
@subsection mlock - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int mlock(
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
@subsection munlock - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int munlock(
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
@subsection mmap - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int mmap(
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
@subsection munmap - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int munmap(
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
@subsection mprotect - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int mprotect(
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
@subsection msync - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int msync(
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
@subsection shm_open - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int shm_open(
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
@subsection shm_unlink - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int shm_unlink(
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

