@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the memory pool
@c  manager.
@c
@c  $Id$
@c

@chapter Memory Pool Manager

@section Introduction

The 
memory pool manager is ...

The services provided by the memory pool manager are:

@itemize @bullet
@item @code{cre_mpl} - Create Variable-Size Memorypool
@item @code{del_mpl} - Delete Variable-Size Memorypool
@item @code{get_blk} - Get Variable-Size Memory Block Poll and Get Variable-Size Memory Block Get Variable-Size Memory Block with Timeout
@item @code{pget_blk} - Poll and Get Variable-Size Memory Block
@item @code{tget_blk} - Get Variable-Size Memory Block with Timeout
@item @code{rel_blk} - Release Variable-Size Memory Block
@item @code{ref_mpl} - Reference Variable-Size Memorypool Status
@end itemize

@section Background

@section Operations

@section System Calls

This section details the memory pool manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  cre_mpl
@c

@page
@subsection cre_mpl - Create Variable-Size Memorypool

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int cre_mpl(
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
@c  del_mpl
@c

@page
@subsection del_mpl - Delete Variable-Size Memorypool

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int del_mpl(
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
@c  get_blk
@c

@page
@subsection get_blk - Get Variable-Size Memory Block Poll and Get Variable-Size Memory Block Get Variable-Size Memory Block with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int get_blk(
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
@c  pget_blk
@c

@page
@subsection pget_blk - Poll and Get Variable-Size Memory Block

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int pget_blk(
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
@c  tget_blk
@c

@page
@subsection tget_blk - Get Variable-Size Memory Block with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int tget_blk(
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
@c  rel_blk
@c

@page
@subsection rel_blk - Release Variable-Size Memory Block

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int rel_blk(
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
@c  ref_mpl
@c

@page
@subsection ref_mpl - Reference Variable-Size Memorypool Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ref_mpl(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:

