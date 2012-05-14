@c
@c COPYRIGHT (c) 1988-2002.
@c On-Line Applications Research Corporation (OAR).
@c All rights reserved.

@chapter Thread Cancellation Manager

@section Introduction

The
thread cancellation manager is ...

The directives provided by the thread cancellation manager are:

@itemize @bullet
@item @code{pthread_cancel} - Cancel Execution of a Thread
@item @code{pthread_setcancelstate} - Set Cancelability State
@item @code{pthread_setcanceltype} - Set Cancelability Type
@item @code{pthread_testcancel} - Create Cancellation Point
@item @code{pthread_cleanup_push} - Establish Cancellation Handler
@item @code{pthread_cleanup_pop} - Remove Cancellation Handler
@end itemize

@section Background

There is currently no text in this section.

@section Operations

There is currently no text in this section.

@section Directives

This section details the thread cancellation manager's directives.
A subsection is dedicated to each of this manager's directives
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c
@c
@page
@subsection pthread_cancel - Cancel Execution of a Thread

@findex pthread_cancel
@cindex  cancel execution of a thread

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pthread_cancel(
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
@subsection pthread_setcancelstate - Set Cancelability State

@findex pthread_setcancelstate
@cindex  set cancelability state

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pthread_setcancelstate(
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
@subsection pthread_setcanceltype - Set Cancelability Type

@findex pthread_setcanceltype
@cindex  set cancelability type

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pthread_setcanceltype(
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
@subsection pthread_testcancel - Create Cancellation Point

@findex pthread_testcancel
@cindex  create cancellation point

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pthread_testcancel(
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
@subsection pthread_cleanup_push - Establish Cancellation Handler

@findex pthread_cleanup_push
@cindex  establish cancellation handler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pthread_cleanup_push(
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
@subsection pthread_cleanup_pop - Remove Cancellation Handler

@findex pthread_cleanup_pop
@cindex  remove cancellation handler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pthread_cleanup_push(
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

