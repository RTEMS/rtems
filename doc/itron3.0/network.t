@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the network support
@c  manager.
@c
@c  $Id$
@c

@chapter Network Support Manager

@section Introduction

The 
network support manager is ...

The services provided by the network support manager are:

@itemize @bullet
@item @code{nrea_dat} - Read Data from another Node
@item @code{nwri_dat} - Write Data to another Node
@item @code{nget_nod} - Get Local Node Number
@item @code{nget_ver} - Get Version Information of another Node
@end itemize

@section Background

@section Operations

@section System Calls

This section details the network support manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  nrea_dat
@c

@page
@subsection nrea_dat - Read Data from another Node

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER nrea_dat(
  INT *p_reasz,
  VP dstadr,
  NODE srcnode,
  VP srcadr,
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
@c  nwri_dat
@c

@page
@subsection nwri_dat - Write Data to another Node

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER nwri_dat(
  INT *p_wrisz,
  NODE dstnode,
  VP dstadr,
  VP srcadr,
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
@c  nget_nod
@c

@page
@subsection nget_nod - Get Local Node Number

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER nget_nod(
  NODE *p_node
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
@c  nget_ver
@c

@page
@subsection nget_ver - Get Version Information of another Node

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER nget_ver(
  T_VER *pk_ver,
  NODE node
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:

