@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Semaphores Manager

@section Introduction

The 
semaphore manager is ...

The directives provided by the semaphore manager are:

@itemize @bullet
@item @code{sem_init} - 
@item @code{sem_destroy} - 
@item @code{sem_open} - 
@item @code{sem_close} - 
@item @code{sem_unlink} - 
@item @code{sem_wait} - 
@item @code{sem_trywait} - 
@item @code{sem_post} - 
@item @code{sem_getvalue} - 
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the semaphore manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection sem_init - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_init(
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
@subsection sem_destroy - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_destroy(
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
@subsection sem_open - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_open(
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
@subsection sem_close - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_close(
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
@subsection sem_unlink - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_unlink(
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
@subsection sem_wait - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_wait(
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
@subsection sem_trywait - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_trywait(
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
@subsection sem_post - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_post(
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
@subsection sem_getvalue - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int sem_getvalue(
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

