@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
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
mailbox manager is basically a linked list, hidden by the super core message queue and consists of a control block, a private structure. The control block comprises of the create mailbox structure, the message structure and the reference mailbox structure.

The services provided by the mailbox manager are:

@itemize @bullet
@item @code{cre_mbx} - Create Mailbox
@item @code{del_mbx} - Delete Mailbox
@item @code{snd_msg} - Send Message to Mailbox
@item @code{rcv_msg} - Receive Message from Mailbox
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
ER cre_mbx(
  ID      mbxid,
  T_CMBX *pk_cmbx
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal completion@*
@code{E_NOMEM} - Insufficient memory@*
@code{E_ID} - Invalid ID number@*
@code{E_RSATR} - Reserved attribute@*
@code{E_OBJ} - Invalid object state@*
@code{E_OACV} - Object access violation@*
@code{E_PAR} - Parameter error


@subheading DESCRIPTION:

Allocated a control area/buffer space for mailbox with some ID.

@example
		User area: 	+ve ids
		System area: 	-ve ids
@end example

User may specify if its FIFO or priority level queue.
Assumes shared memory b/w communicating processes.
Initializes core message queue for this mbox.

@subheading NOTES:

NONE


@c
@c  del_mbx
@c

@page

@subsection del_mbx - Delete Mailbox

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER del_mbx(
  ID mbxid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal completion@*
@code{E_ID} - Invalid ID number@*
@code{E_NOEXS} - Object does not exist@*
@code{E_OACV} - Object access violation

@subheading DESCRIPTION:

Specified by the ID, cleans up all data structures and control blocks.

@subheading NOTES:

NONE


@c
@c  snd_msg
@c

@page

@subsection snd_msg - Send Message to Mailbox

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER snd_msg(
  ID     mbxid,
  T_MSG *pk_msg
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal completion@*
@code{E_ID} - Invalid ID number@*
@code{E_NOEXS} - Object does not exist@*
@code{E_OACV} - Object access violation@*
@code{E_QOVR} - Queueing or nesting overflow

@subheading DESCRIPTION:

Sends the address of message to mbox having a given id, any waiting tasks (blocked tasks) will be woken up. It supports non-blocking send.

@subheading NOTES:

NONE


@c
@c  rcv_msg
@c

@page

@subsection rcv_msg - Receive Message from Mailbox

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER rcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal completion@*
@code{E_ID} - Invalid ID number@*
@code{E_NOEXS} - Object does not exist@*
@code{E_OACV} - Object access violation@*
@code{E_PAR} - Parameter error@*
@code{E_DLT} - The object being waited for was deleted@*
@code{E_RLWAI} - WAIT state was forcibly released@*
@code{E_CTX} - Context error

@subheading DESCRIPTION:

If there is no message then receiver blocks, if not empty then it takes the first message of the queue.

@subheading NOTES:

NONE


@c
@c  prcv_msg
@c

@page

@subsection prcv_msg - Poll and Receive Message from Mailbox

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER prcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal completion@*
@code{E_ID} - Invalid ID number@*
@code{E_NOEXS} - Object does not exist@*
@code{E_OACV} - Object access violation@*
@code{E_PAR} - Parameter error@*
@code{E_DLT} - The object being waited for was deleted@*
@code{E_RLWAI} - WAIT state was forcibly released@*
@code{E_CTX} - Context error@*

@subheading DESCRIPTION:

Poll and receive message from mailbox.

@subheading NOTES:

NONE


@c
@c  trcv_msg
@c

@page

@subsection trcv_msg - Receive Message from Mailbox with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER trcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid,
  TMO     tmout
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal completion@*
@code{E_ID} - Invalid ID number@*
@code{E_NOEXS} - Object does not exist@*
@code{E_OACV} - Object access violation@*
@code{E_PAR} - Parameter error@*
@code{E_DLT} - The object being waited for was deleted@*
@code{E_RLWAI} - WAIT state was forcibly released@*
@code{E_CTX} - Context error

@subheading DESCRIPTION:

Blocking receive with a maximum timeout.

@subheading NOTES:

NONE


@c
@c  ref_mbx
@c

@page

@subsection ref_mbx - Reference Mailbox Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_mbx(
  T_RMBX *pk_rmbx,
  ID      mbxid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal completion@*
@code{E_ID} - Invalid ID number@*
@code{E_NOEXS} - Object does not exist@*
@code{E_OACV} - Object access violation@*
@code{E_PAR} - Parameter error

@subheading DESCRIPTION:

Supports non-blocking receive. If there are no messages, it returns -1. Also returns id of the next process waiting on a message.

@subheading NOTES:

NONE


