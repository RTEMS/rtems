@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
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
@item @code{def_int} - Define Interrupt Handler
@item @code{ret_int} - Return from Interrupt Handler
@item @code{ret_wup} - Return and Wakeup Task
@item @code{loc_cpu} - Lock CPU
@item @code{unl_cpu} - Unlock CPU
@item @code{dis_int} - Disable Interrupt
@item @code{ena_int} - Enable Interrupt
@item @code{chg_iXX} - Change Interrupt Mask(Level or Priority)
@item @code{ref_iXX} - Reference Interrupt Mask(Level or Priority)
@end itemize

@section Background

@section Operations

@section System Calls

This section details the interrupt manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  def_int
@c

@page
@subsection def_int - Define Interrupt Handler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER def_int(
  UINT dintno,
  T_DINT *pk_dint
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
@c  ret_int
@c

@page
@subsection ret_int - Return from Interrupt Handler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void ret_int(

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
@c  ret_wup
@c

@page
@subsection ret_wup - Return and Wakeup Task

@subheading CALLING SEQUENCE:

@ifset is-C
@example
void ret_wup(
  ID tskid
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
@c  loc_cpu
@c

@page
@subsection loc_cpu - Lock CPU

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER loc_cpu(

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
@c  unl_cpu
@c

@page
@subsection unl_cpu - Unlock CPU

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER unl_cpu(

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
@c  dis_int
@c

@page
@subsection dis_int - Disable Interrupt

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER dis_int(
  UINT eintno
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
@c  ena_int
@c

@page
@subsection ena_int - Enable Interrupt

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ena_int(
  UINT eintno
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
@c  chg_iXX
@c

@page
@subsection chg_iXX - Change Interrupt Mask(Level or Priority)

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER chg_iXX(
  UINT iXXXX
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
@c  ref_iXX
@c

@page
@subsection ref_iXX - Reference Interrupt Mask(Level or Priority)

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_iXX(
  UINT *p_iXXXX
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:

