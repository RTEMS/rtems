@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the mailbox
@c  manager.
@c
@c  $Id$
@c

@chapter Mailbox Manager

@section Introduction

The 
mailbox manager is ...

The services provided by the mailbox manager are:

@itemize @bullet
@item @code{cre_mbx} - Create Mailbox
@item @code{del_mbx} - Delete Mailbox
@item @code{snd_msg} - Send Message to Mailbox
@item @code{rcv_msg} - Poll and Receive Message from Mailbox Receive Message from Mailbox Receive Message from Mailbox with Timeout
@item @code{prcv_msg} - Poll and Receive Message from Mailbox
@item @code{trcv_msg} - Receive Message from Mailbox with Timeout
@item @code{ref_mbx} - Reference Mailbox Status
@end itemize

@section Background

@section Operations

@section System Calls

This section details the mailbox manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  cre_mbx
@c

@page
@subsection cre_mbx - Create Mailbox

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER cre_mbx (
 ID mbxid, T_CMBX* pk_cmbx
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
@c  del_mbx
@c

@page
@subsection del_mbx - Delete Mailbox

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER del_mbx (
 ID mbxid
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
@c  snd_msg
@c

@page
@subsection snd_msg - Send Message to Mailbox

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER snd_msg (
 ID mbxid, T_MSG *pk_msg
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
@c  rcv_msg
@c

@page
@subsection rcv_msg - Poll and Receive Message from Mailbox Receive Message from Mailbox Receive Message from Mailbox with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER rcv_msg (
 T_MSG **ppk_msg, ID mbxid ER ercd =prcv_msg 
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
@c  prcv_msg
@c

@page
@subsection prcv_msg - Poll and Receive Message from Mailbox

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =prcv_msg (
 T_MSG **ppk_msg, ID mbxid
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
@c  trcv_msg
@c

@page
@subsection trcv_msg - Receive Message from Mailbox with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =trcv_msg (
 T_MSG **ppk_msg, ID mbxid, TMO tmout
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
@c  ref_mbx
@c

@page
@subsection ref_mbx - Reference Mailbox Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_mbx (
 T_RMBX *pk_rmbx, ID mbxid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{EXXX} - 

@subheading DESCRIPTION:

@subheading NOTES:

