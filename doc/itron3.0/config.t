@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the system
@c  manager.
@c
@c  $Id$
@c

@chapter System Manager

@section Introduction

The 
system manager is ...

The services provided by the system manager are:

@itemize @bullet
@item @code{get_ver} - Get Version Information
@item @code{ref_sys} - Reference Semaphore Status
@item @code{ref_cfg} - Reference Configuration Information
@item @code{def_svc} - Define Extended SVC Handler
@item @code{def_exc} - Define Exception Handler
@end itemize

@section Background

@section Operations

@section System Calls

This section details the system manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  get_ver
@c

@page
@subsection get_ver - Get Version Information 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER get_ver(
 T_VER *pk_ver
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
@c  ref_sys
@c

@page
@subsection ref_sys - Reference Semaphore Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_sys(
 T_RSYS *pk_rsys
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
@c  ref_cfg
@c

@page
@subsection ref_cfg - Reference Configuration Information

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_cfg(
 T_RCFG *pk_rcfg
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
@c  def_svc
@c

@page
@subsection def_svc - Define Extended SVC Handler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER def_svc(
  FN s_fncd,
  T_DSVC *pk_dsvc
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
@c  def_exc
@c

@page
@subsection def_exc - Define Exception Handler

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER def_exc(
  UINT exckind,
  T_DEXC *pk_dexc
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:

