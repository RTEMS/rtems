@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the message buffer
@c  manager.
@c
@c  $Id$
@c

@chapter Message Buffer Manager

@section Introduction

The 
message buffer manager is ...

The services provided by the message buffer manager are:

@itemize @bullet
@item @code{cre_mbf} - Create MessageBuffer
@item @code{del_mbf} - Delete MessageBuffer
@item @code{snd_mbf} - Poll and Send Message to MessageBuffer Send Message to MessageBuffer Send Message to MessageBuffer with Timeout
@item @code{psnd_mbf} - Poll and Send Message to MessageBuffer
@item @code{tsnd_mbf} - Send Message to MessageBuffer with Timeout
@item @code{rcv_mbf} - Poll and Receive Message from MessageBuffer Receive Message from MessageBuffer Receive Message from MessageBuffer with Timeout
@item @code{prcv_mbf} - Poll and Receive Message from MessageBuffer
@item @code{trcv_mbf} - Receive Message from MessageBuffer with Timeout
@item @code{ref_mbf} - Reference MessageBuffer Status
@end itemize

@section Background

@section Operations

@section System Calls

This section details the message buffer manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  cre_mbf
@c

@page
@subsection cre_mbf - Create MessageBuffer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cre_mbf(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  del_mbf
@c

@page
@subsection del_mbf - Delete MessageBuffer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int del_mbf(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  snd_mbf
@c

@page
@subsection snd_mbf - Poll and Send Message to MessageBuffer Send Message to MessageBuffer Send Message to MessageBuffer with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int snd_mbf(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  psnd_mbf
@c

@page
@subsection psnd_mbf - Poll and Send Message to MessageBuffer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int psnd_mbf(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  tsnd_mbf
@c

@page
@subsection tsnd_mbf - Send Message to MessageBuffer with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int tsnd_mbf(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  rcv_mbf
@c

@page
@subsection rcv_mbf - Poll and Receive Message from MessageBuffer Receive Message from MessageBuffer Receive Message from MessageBuffer with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int rcv_mbf(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  prcv_mbf
@c

@page
@subsection prcv_mbf - Poll and Receive Message from MessageBuffer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int prcv_mbf(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  trcv_mbf
@c

@page
@subsection trcv_mbf - Receive Message from MessageBuffer with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int trcv_mbf(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:


@c
@c  ref_mbf
@c

@page
@subsection ref_mbf - Reference MessageBuffer Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ref_mbf(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:

