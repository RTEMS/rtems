@c
@c  COPYRIGHT (c) 1988-1998.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved. 
@c
@c  $Id$
@c

@chapter Input and Output Primitives Manager

@section Introduction

The 
input and output primitives manager is ...

The directives provided by the input and output primitives manager are:

@itemize @bullet
@item @code{pipe} - 
@item @code{dup} - 
@item @code{dup2} - 
@item @code{close} - 
@item @code{read} - 
@item @code{write} - 
@item @code{fcntl} - 
@item @code{lseek} - 
@item @code{fsynch} - 
@item @code{fdatasynch} - 
@item @code{aio_read} - 
@item @code{aio_write} - 
@item @code{lio_listio} - 
@item @code{aio_error} - 
@item @code{aio_return} - 
@item @code{aio_cancel} - 
@item @code{aio_suspend} - 
@item @code{aio_fsync} - 
@end itemize

@section Background

@section Operations

@section Directives

This section details the input and output primitives manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@page
@subsection pipe - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pipe(
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
@subsection dup - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int dup(
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
@subsection dup2 - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int dup2(
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
@subsection close - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int close(
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
@subsection read - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int read(
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
@subsection write - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int write(
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
@subsection fcntl - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fcntl(
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
@subsection lseek - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int lseek(
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
@subsection fsynch - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fsynch(
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
@subsection fdatasynch - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int fdatasynch(
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
@subsection aio_read - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_read(
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
@subsection aio_write - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_write(
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
@subsection lio_listio - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int lio_listio(
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
@subsection aio_error - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_error(
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
@subsection aio_return - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_return(
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
@subsection aio_cancel - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_cancel(
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
@subsection aio_suspend - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_suspend(
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
@subsection aio_fsync - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int aio_fsync(
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

