@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the rendezvous
@c  manager.
@c
@c  $Id$
@c

@chapter Rendezvous Manager

@section Introduction

The 
rendezvous manager is ...

The services provided by the rendezvous manager are:

@itemize @bullet
@item @code{cre_por} - Create Port for Rendezvous
@item @code{del_por} - Delete Port for Rendezvous
@item @code{cal_por} - Call Port for Rendezvous
@item @code{pcal_por} - Poll and Call Port for Rendezvous
@item @code{tcal_por} - Call Port for Rendezvous with Timeout
@item @code{acp_por} - Accept Port for Rendezvous
@item @code{pacp_por} - Poll and Accept Port for Rendezvous
@item @code{tacp_por} - Accept Port for Rendezvous with Timeout
@item @code{fwd_por} - Forward Rendezvous to Other Port
@item @code{rpl_rdv} - Reply Rendezvous
@item @code{ref_por} - Reference Port Status
@end itemize

@section Background

@section Operations

@section System Calls

This section details the rendezvous manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  cre_por
@c

@page
@subsection cre_por - Create Port for Rendezvous

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER cre_por(
  ID porid,
  T_CPOR *pk_cpor
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
@c  del_por
@c

@page
@subsection del_por - Delete Port for Rendezvous

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER del_por(
  ID porid
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
@c  cal_por
@c

@page
@subsection cal_por - Call Port for Rendezvous Poll 

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER cal_por(
  VP msg,
  INT *p_rmsgsz,
  ID porid,
  UINT calptn
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
@c  pcal_por
@c

@page
@subsection pcal_por - Poll and Call Port for Rendezvous

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =pcal_por(
  VP msg,
  INT *p_rmsgsz,
  ID porid,
  UINT calptn,
  INT
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
@c  tcal_por
@c

@page
@subsection tcal_por - Call Port for Rendezvous with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =tcal_por(
  VP msg,
  INT *p_rmsgsz,
  ID porid,
  UINT calptn,
  INT
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
@c  acp_por
@c

@page
@subsection acp_por - Accept Port for Rendezvous Poll

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER acp_por(
  RNO *p_rdvno,
  VP msg,
  INT *p_cmsgsz,
  ID porid
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
@c  pacp_por
@c

@page
@subsection pacp_por - Poll and Accept Port for Rendezvous

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =pacp_por(
  RNO *p_rdvno,
  VP msg,
  INT *p_cmsgsz,
  ID porid,
  UINT
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
@c  tacp_por
@c

@page
@subsection tacp_por - Accept Port for Rendezvous with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =tacp_por(
  RNO *p_rdvno,
  VP msg,
  INT *p_cmsgsz,
  ID porid,
  UINT
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
@c  fwd_por
@c

@page
@subsection fwd_por - Forward Rendezvous to Other Port

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER fwd_por(
  ID porid,
  UINT calptn,
  RNO rdvno,
  VP msg,
  INT cmsgsz
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
@c  rpl_rdv
@c

@page
@subsection rpl_rdv - Reply Rendezvous

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER rpl_rdv(
  RNO rdvno,
  VP msg,
  INT rmsgsz
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
@c  ref_por
@c

@page
@subsection ref_por - Reference Port Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_por(
  T_RPOR *pk_rpor,
  ID porid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:

