@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the interrupt
@c  manager.
@c
@c  $Id$
@c

@chapter Interrupt Manager

@section Introduction

The 
interrupt manager is ...

The services provided by the interrupt manager are:

@itemize @bullet
@item @code{def_int} - 
@item @code{ret_int} - 
@item @code{ret_wup} - 
@item @code{loc_cpu} - 
@item @code{unl_cpu} - 
@item @code{dis_int} - 
@item @code{ena_int} - 
@item @code{chg_iXX} - 
@item @code{ref_iXX} - 
@end itemize

@section Background

@section Operations

@section Directives

This section details the interrupt manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  def_int
@c

@page
@subsection def_int - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int def_int(
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
@c  ret_int
@c

@page
@subsection ret_int - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ret_int(
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
@c  ret_wup
@c

@page
@subsection ret_wup - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ret_wup(
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
@c  loc_cpu
@c

@page
@subsection loc_cpu - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int loc_cpu(
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
@c  unl_cpu
@c

@page
@subsection unl_cpu - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int unl_cpu(
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
@c  dis_int
@c

@page
@subsection dis_int - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int dis_int(
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
@c  ena_int
@c

@page
@subsection ena_int - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ena_int(
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
@c  chg_iXX
@c

@page
@subsection chg_iXX - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int chg_iXX(
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
@c  ref_iXX
@c

@page
@subsection ref_iXX - 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ref_iXX(
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

