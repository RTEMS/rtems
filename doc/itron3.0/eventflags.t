@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the eventflags
@c  manager.
@c
@c  $Id$
@c

@chapter Eventflags Manager

@section Introduction

The 
eventflags manager is ...

The services provided by the eventflags manager are:

@itemize @bullet
@item @code{cre_flg} - Create Eventflag
@item @code{del_flg} - Delete Eventflag
@item @code{set_flg} - Set Eventflag
@item @code{clr_flg} - Clear Eventflag
@item @code{wai_flg} - Wait on Eventflag
@item @code{pol_flg} - Wait for Eventflag(Polling)
@item @code{twai_flg} - Wait on Eventflag with Timeout
@item @code{ref_flg} - Reference Eventflag Status
@end itemize

@section Background

@section Operations

@section System Calls

This section details the eventflags manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  cre_flg
@c

@page
@subsection cre_flg - Create Eventflag

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER cre_flg(
  ID flgid,
  T_CFLG *pk_cflg
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
@c  del_flg
@c

@page
@subsection del_flg - Delete Eventflag

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER del_flg(
  ID flgid
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
@c  set_flg
@c

@page
@subsection set_flg - Set Eventflag

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER set_flg(
  ID flgid,
  UINT setptn
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
@c  clr_flg
@c

@page
@subsection clr_flg - Clear Eventflag

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER clr_flg(
  ID flgid,
  UINT clrptn
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
@c  wai_flg
@c

@page
@subsection wai_flg - Wait on Eventflag

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER wai_flg(
  UINT *p_flgptn,
  ID flgid,
  UINT waiptn,
  UINT wfmode
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
@c  pol_flg
@c

@page
@subsection pol_flg - Wait for Eventflag(Polling)

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER pol_flg(
  UINT *p_flgptn,
  ID flgid,
  UINT waiptn,
  UINT wfmode
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
@c  twai_flg
@c

@page
@subsection twai_flg - Wait on Eventflag with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =twai_flg(
  UINT *p_flgptn,
  ID flgid,
  UINT waiptn,
  UINT wfmode,
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
@c  ref_flg
@c

@page
@subsection ref_flg - Reference Eventflag Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_flg(
  T_RFLG *pk_rflg,
  ID flgid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:

