@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the time
@c  manager.
@c
@c  $Id$
@c

@chapter Time Manager

@section Introduction

The 
time manager is ...

The services provided by the time manager are:

@itemize @bullet
@item @code{get_tim} - Get System Clock
@item @code{set_tim} - Set System Clock
@item @code{dly_tsk} - Delay Task
@item @code{def_cyc} - Define Cyclic Handler
@item @code{act_cyc} - Activate Cyclic Handler
@item @code{ref_cyc} - Reference Cyclic Handler Status
@item @code{def_alm} - Define Alarm Handler
@item @code{ref_alm} - Reference Alarm Handler Status
@item @code{ret_tmr} - Return from Timer Handler
@end itemize

@section Background

@section Operations

@section System Calls

This section details the time manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  get_tim
@c

@page
@subsection get_tim - Get System Clock

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int get_tim(
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
@c  set_tim
@c

@page
@subsection set_tim - Set System Clock

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int set_tim(
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
@c  dly_tsk
@c

@page
@subsection dly_tsk - Delay Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int dly_tsk(
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
@c  def_cyc
@c

@page
@subsection def_cyc - Define Cyclic Handler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int def_cyc(
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
@c  act_cyc
@c

@page
@subsection act_cyc - Activate Cyclic Handler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int act_cyc(
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
@c  ref_cyc
@c

@page
@subsection ref_cyc - Reference Cyclic Handler Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ref_cyc(
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
@c  def_alm
@c

@page
@subsection def_alm - Define Alarm Handler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int def_alm(
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
@c  ref_alm
@c

@page
@subsection ref_alm - Reference Alarm Handler Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ref_alm(
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
@c  ret_tmr
@c

@page
@subsection ret_tmr - Return from Timer Handler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
int ret_tmr(
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:

