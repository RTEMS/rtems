@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the fixed block
@c  manager.
@c
@c  $Id$
@c

@chapter Fixed Block Manager

@section Introduction

The 
fixed block manager is ...

The services provided by the fixed block manager are:

@itemize @bullet
@item @code{cre_mpf} - Create Fixed-Size Memorypool
@item @code{del_mpf} - Delete Fixed-Size Memorypool
@item @code{get_blf} - Get Fixed-Size Memory Block Poll and Get Fixed-Size Memory Block Get Fixed-Size Memory Block with Timeout
@item @code{pget_blf} - Poll and Get Fixed-Size Memory Block
@item @code{tget_blf} - Get Fixed-Size Memory Block with Timeout
@item @code{rel_blf} - Release Fixed-Size Memory Block
@item @code{ref_mpf} - Reference Fixed-Size Memorypool Status
@end itemize

@section Background

@section Operations

@section System Calls

This section details the fixed block manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  cre_mpf
@c

@page
@subsection cre_mpf - Create Fixed-Size Memorypool

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER cre_mpf (
 ID mpfid, T_CMPF *pk_cmpf
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
@c  del_mpf
@c

@page
@subsection del_mpf - Delete Fixed-Size Memorypool

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER del_mpf (
 ID mpfid
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
@c  get_blf
@c

@page
@subsection get_blf - Get Fixed-Size Memory Block Poll and Get Fixed-Size Memory Block Get Fixed-Size Memory Block with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER get_blf (
 VP *p_blf, ID mpfid ER ercd =pget_blf 
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
@c  pget_blf
@c

@page
@subsection pget_blf - Poll and Get Fixed-Size Memory Block

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =pget_blf (
 VP *p_blf, ID mpfid
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
@c  tget_blf
@c

@page
@subsection tget_blf - Get Fixed-Size Memory Block with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =tget_blf (
 VP *p_blf, ID mpfid, TMO tmout
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
@c  rel_blf
@c

@page
@subsection rel_blf - Release Fixed-Size Memory Block

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER rel_blf (
 ID mpfid, VP blf
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
@c  ref_mpf
@c

@page
@subsection ref_mpf - Reference Fixed-Size Memorypool Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_mpf (
 T_RMPF *pk_rmpf, ID mpfid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:

