@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the message buffer
@c  manager.
@c
@c  $Id$
@c

@chapter Message Buffer Manager

@section Introduction

The message buffer manager provides functions to create, delete, and
control of message buffers.  This manager is based on the ITRON 3.0
standard. 

The services provided by the message buffer manager are:

@itemize @bullet
@item @code{cre_mbf} - Create MessageBuffer
@item @code{del_mbf} - Delete MessageBuffer
@item @code{snd_mbf} - Send Message to MessageBuffer
@item @code{psnd_mbf} - Poll and Send Message to MessageBuffer
@item @code{tsnd_mbf} - Send Message to MessageBuffer with Timeout
@item @code{rcv_mbf} - Receive Message from MessageBuffer
@item @code{prcv_mbf} - Poll and Receive Message from MessageBuffer
@item @code{trcv_mbf} - Receive Message from MessageBuffer with Timeout
@item @code{ref_mbf} - Reference MessageBuffer Status
@end itemize

@section Background

@subsection T_CMBF Structure

The T_CMBF structure is used to define the characteristics of a message
buffer and passed as an argument to the @code{cre_mbf} routine. This
structure is defined as:

@example
@group

typedef struct t_cmbf @{
  VP      exinf;   /* extended information */
  ATR     mbfatr;  /* message buffer attributes */
  INT     bufsz;   /* buffer size (in bytes) */
  INT     maxmsz;  /* maximum message size (in bytes) */
  /* (CPU and/or implementation-dependent information may also be
  included) */
@} T_CMBF;

@end group
@end example

where the meaning of each field is:

@table @b
@item exinf
is for any extended information that the implementation may define. This
implementation does not use this field.

@item mbfatr
is the attributes for the message buffer. The only attributes which can
be specified is whether tasks wait in FIFO (@code{TA_TFIFO}) or priority
(@code{TA_TPRI}) order.

@item bufsz
is the size of the message buffer. Since some control data are needed to
manage each messages in the message buffer, @code{bufsz} is usually
larger than the total of all message sizes in this buffer.

@item maxmsz
is the maximum message size.

@end table

@subsection T_RMBF Structure

The T_RMBF structure is filled in by the @code{ref_mbf} routine with
status and state information of the message buffer. The structure is
defined as follows:

@example
@group

typedef struct t_rmbf @{
  VP      exinf;   /* extended information */
  BOOL_ID wtsk;    /* waiting task information */
  BOOL_ID stsk;    /* sending task information */
  INT     msgsz;   /* message size (in bytes) */
  INT     frbufsz; /* free buffer size (in bytes) */
  /* (CPU and/or implementation-dependent information is returned) */
@} T_RMBF;

@end group
@end example

@table @b

@item exinf
is for any extended information that the implementation may define.
This implementation does not use this field.

@item wtsk
is TRUE when there is one or more tasks waiting on this message buffer
to send message. It is FALSE when there is no waiting task. The meaning
of this field is allowed to vary between ITRON implementations.  It may
have the ID of a waiting task, the number of tasks waiting, or a boolean
indication that one or more tasks are waiting.

@item stsk
is TRUE when there is one or more tasks waiting on this message buffer
to receive message. It is FALSE when there is no waiting task. The meaning
of this field is allowed to vary between ITRON implementations.  It may
have the ID of a waiting task, the number of tasks waiting, or a boolean
indication that one or more tasks are waiting.

@item msgsz
is the size of the message that is at the head of the message buffer. If
there is no message on the message queue, @code{msgsz} will be returned
as FALSE = 0.

@item frbufsz
is the amount of free memory in the message buffer. 

@end table

@section Operations

@section System Calls

This section details the message buffer manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.

@c
@c  cre_mbf
@c

@page
@subsection cre_mbf - Create MessageBuffer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER cre_mbf(
  ID mbfid,
  T_CMBF *pk_cmbf
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_NOMEM} - Insufficient memory (Memory for control block and/or
ring buffer cannot be allocted)

@code{E_ID} - Invalid ID number (mbfid was invalid or could not be used)

@code{E_RSATR} - Reserved attribute (mbfatr was invalid or could not be
used)

@code{E_OBJ} - Invalid object state (a messagebuffer of the same ID
already exists)

@code{E_OACV} - Object access violation (A mbfid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (pk_cmbf is invalid or bufsz and/or
maxmsz is negative or invalid) 

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.


@code{EN_CTXID} - Specified an object on another node when the system
call was issued from a task in dispatch disabled state or from a
task-independent portion

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for exinf, mbfatr, bufsz and/or
maxmsz) 

@subheading DESCRIPTION:

This routine creates a message buffer on the local node. The message
buffer is initialized based on the attributes specified in the
@code{pk_cmbf} structure. The buffer size and the maximum message size
are determined by the @code{bufsz} and @code{maxmsz} fields in this
structure. 

The @code{mbfatr} field represents attributes of the message
buffer. If @code{TA_TFIFO} is specified, tasks will be put on the queue
on a First In-First Out basis. If @code{TA_TPRI} is specified, tasks
will be placed on the queue according to their priority.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

@c
@c  del_mbf
@c

@page
@subsection del_mbf - Delete MessageBuffer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER del_mbf(
  ID mbfid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (mbfid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the messagebuffer specified by
mbfid does not exist)

@code{E_OACV} - Object access violation (A mbfid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system
call was issued from a task in dispatch disabled state or from a
task-independent portion 

@subheading DESCRIPTION:

This routine deletes the message buffer specified by @code{mbfid}. 
Issuing this system call releases memory area used for the control block of
the associated message buffer and the buffer area used for storing messages.

This routine will complete normally even if there are tasks waiting to
send or receive messages at the message buffer.  In that case, an
@code{E_DLT} error will be returned to each waiting task.  If there are
messages still in the message buffer, they will be deleted along with
the message buffer and no error will result.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

@c
@c  snd_mbf
@c

@page
@subsection snd_mbf - Send Message to Message Buffer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER snd_mbf(
  ID mbfid,
  VP msg,
  INT msgsz
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (mbfid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the message buffer specified by
mbfid does not exist)

@code{E_OACV} - Object access violation (A mbfid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (msgsz is 0 or less; msgsz is larger than
maxmsz; values unsuitable for msg; tmout is -2 or less) 

@code{E_DLT} - The object being waited for was deleted (the
message buffer of interest was deleted while waiting)

@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received
while waiting)

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state; implementation dependent for psnd_mbf
and tsnd_mbf(tmout=TMO_POL)) 

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system
call was issued from a task in dispatch disabled state or from a
task-independent portion (implementation-dependent; applicable to
psnd_mbf and tsnd_mbf (tmout=TMO_POL) only) 

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for msgsz and/or tmout) 

@subheading DESCRIPTION:

This routine sends the message stored at the address given by @code{msg}
to the message buffer specified by @code{mbfid}.  The size of the
message is specified by @code{msgsz}; that is, @code{msgsz} number of
bytes beginning from @code{msg} are copied to the message buffer
specified by @code{mbfid}.  If the available space in the buffer is not
enough to include the message given by @code{msg}, the task issuing this
system call will wait on a send wait queue until more buffer space
becomes available.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

@c
@c  psnd_mbf
@c

@page
@subsection psnd_mbf - Poll and Send Message to Message Buffer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =psnd_mbf(
  ID mbfid,
  VP msg,
  INT msgsz
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (mbfid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the message buffer specified by
mbfid does not exist)

@code{E_OACV} - Object access violation (A mbfid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (msgsz is 0 or less; msgsz is larger than
maxmsz; values unsuitable for msg; tmout is -2 or less) 

@code{E_DLT} - The object being waited for was deleted (the
message buffer of interest was deleted while waiting)

@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received
while waiting)

@code{E_TMOUT} - Polling failure or timeout

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state; implementation dependent for psnd_mbf
and tsnd_mbf(tmout=TMO_POL)) 

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system
call was issued from a task in dispatch disabled state or from a
task-independent portion (implementation-dependent; applicable to
psnd_mbf and tsnd_mbf (tmout=TMO_POL) only) 

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for msgsz and/or tmout) 

@subheading DESCRIPTION:

This routine has the same function as @code{snd_mbf} except for the
waiting feature.  @code{Psnd_mbf} polls whether or not the task should
wait if @code{snd_mbf} is executed.  The meaning of parameters to
@code{psnd_mbf} are the same as for @code{snd_mbf}.  The specific
operations by @code{psnd_mbf} are as follows.

  - If there is enough space in the buffer, processing is the same as
  @code{snd_mbf}: the message is sent and the system call completes
  normally.

  - If there is not enough space in the buffer, an @code{E_TMOUT} error
  is returned to indicate polling failed and the system call finishes.
  Unlike @code{snd_mbf}, the issuing task does not wait in this case.
  The status of the message buffer and the message queue remain unchanged.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

@c
@c  tsnd_mbf
@c

@page
@subsection tsnd_mbf - Send Message to Message Buffer with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =tsnd_mbf(
  ID mbfid,
  VP msg,
  INT msgsz,
  TMO tmout
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (mbfid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the message buffer specified by
mbfid does not exist)

@code{E_OACV} - Object access violation (A mbfid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (msgsz is 0 or less; msgsz is larger than
maxmsz; values unsuitable for msg; tmout is -2 or less) 

@code{E_DLT} - The object being waited for was deleted (the
message buffer of interest was deleted while waiting)

@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received
while waiting)

@code{E_TMOUT} - Polling failure or timeout

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state; implementation dependent for psnd_mbf
and tsnd_mbf(tmout=TMO_POL)) 

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system
call was issued from a task in dispatch disabled state or from a
task-independent portion (implementation-dependent; applicable to
psnd_mbf and tsnd_mbf (tmout=TMO_POL) only) 

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for msgsz and/or tmout) 

@subheading DESCRIPTION:

The @code{tsnd_mbf} system call has the same function as @code{snd_mbf}
with an additional timeout feature.  A maximum wait time (timeout value)
can be specified using the parameter @code{tmout}.  When a timeout is
specified, a timeout error, @code{E_TMOUT}, will result and the system
call will finish if the period specified by @code{tmout} elapses without
conditions for releasing wait being satisfied (i.e. without sufficient
buffer space becoming available).

Only positive values can be specified for @code{tmout}.  Specifying
@code{TMO_POL} = 0 to @code{tsnd_mbf} for @code{tmout} indicates that a
timeout value of 0 be used, resulting in exactly the same processing as
@code{psnd_mbf}.  Specifying @code{TMO_FEVR} = -1 to @code{tsnd_mbf} for
@code{tmout} indicates that an infinite timeout value be used, resulting
in exactly the same processing as @code{snd_mbf}.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.


@c
@c  rcv_mbf
@c

@page
@subsection rcv_mbf - Receive Message from Message Buffer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER rcv_mbf(
  VP msg,
  INT *p_msgsz,
  ID mbfid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (mbfid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the message buffer specified by
mbfid does not exist)

@code{E_OACV} - Object access violation (A mbfid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (values unsuitable for msg; tmout is -2
or less) 

@code{E_DLT} - The object being waited for was deleted (the specified
message buffer was deleted while waiting)

@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received
while waiting)

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state) 

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for tmout)

@code{EN_RPAR} - A value outside the range supported by the issuing node
and/or transmission packet format was returned as a return parameter (msgsz is outside supported range for requesting node)

@subheading DESCRIPTION:

@code{Rcv_mbf} receives the message from the message buffer specified by
@code{mbfid}, and stores it at the memory location given by @code{msg}.
In other words, the content of the message at the head of the message
buffer specified by @code{mbfid} is copied into an area which begins
from @code{msg} and whose size is @code{msgsz}.

If the message buffer is empty, the task issuing this system call will wait
on a receive message wait queue until a message arrives.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.


@c
@c  prcv_mbf
@c

@page
@subsection prcv_mbf - Poll and Receive Message from Message Buffer

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =prcv_mbf(
  VP msg,
  INT *p_msgsz,
  ID mbfid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (mbfid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the message buffer specified by
mbfid does not exist)

@code{E_OACV} - Object access violation (A mbfid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (values unsuitable for msg; tmout is -2
or less) 

@code{E_DLT} - The object being waited for was deleted (the specified
message buffer was deleted while waiting)

@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received
while waiting)

@code{E_TMOUT} - Polling failure or timeout

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state) 

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for tmout)

@code{EN_RPAR} - A value outside the range supported by the issuing node
and/or transmission packet format was returned as a return parameter (msgsz is outside supported range for requesting node)


@subheading DESCRIPTION:
The @code{prcv_mbf} system call has the same function as @code{rcv_mbf}
except for the waiting feature.  @code{Prcv_mbf} polls whether or not
the task should wait if @code{rcv_mbf} is executed.  The meaning of
parameters to @code{prcv_mbf} are the same as for @code{rcv_mbf}.  The
specific operations by @code{prcv_mbf} are as follows.

  - If there is a message in the specified message buffer, processing is
the same as @code{rcv_mbf}: the first message on the message buffer is
retrieved and the system call completes normally.

  - If there is no message in the specified message buffer, an
@code{E_TMOUT} error is returned to indicate polling failed and the
system call finishes.  Unlike @code{rcv_mbf}, the issuing task does not
wait in this case.  The status of the message buffer remain unchanged.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.


@c
@c  trcv_mbf
@c

@page
@subsection trcv_mbf - Receive Message from Message Buffer with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ercd =trcv_mbf(
  VP msg,
  INT *p_msgsz,
  ID mbfid,
  TMO tmout
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (mbfid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the message buffer specified by
mbfid does not exist)

@code{E_OACV} - Object access violation (A mbfid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (values unsuitable for msg; tmout is -2
or less) 

@code{E_DLT} - The object being waited for was deleted (the specified
message buffer was deleted while waiting)

@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received
while waiting)

@code{E_TMOUT} - Polling failure or timeout

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state) 

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for tmout)

@code{EN_RPAR} - A value outside the range supported by the issuing node
and/or transmission packet format was returned as a return parameter (msgsz is outside supported range for requesting node)

@subheading DESCRIPTION:
The @code{trcv_mbf} system call has the same function as @code{rcv_mbf}
with an additional timeout feature.  A maximum wait time (timeout value)
can be specified using the parameter @code{tmout}.  When a timeout is
specified, a timeout error, @code{E_TMOUT}, will result and the system
call will finish if the period specified by @code{tmout} elapses without
conditions for releasing wait being satisfied (i.e. without a message
arriving).

Only positive values can be specified for @code{tmout}.  Specifying
@code{TMO_POL} = 0 to @code{trcv_mbf} for @code{tmout} indicates that a
timeout value of 0 be used, resulting in exactly the same processing as
@code{prcv_mbf}.  Specifying @code{TMO_FEVR} = -1 to @code{trcv_mbf} for
tmout indicates that an infinite timeout value be used, resulting in
exactly the same processing as @code{rcv_mbf}.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

@c
@c  ref_mbf
@c

@page
@subsection ref_mbf - Reference Message Buffer Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_mbf(
  T_RMBF *pk_rmbf,
  ID mbfid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (mbfid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the message buffer specified by
mbfid does not exist)

@code{E_OACV} - Object access violation (A mbfid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (the packet address for the return
parameters could not be used) 

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system
call was issued from a task in dispatch disabled state or from a
task-independent portion

@code{EN_RPAR} - A value outside the range supported by the issuing node
and/or transmission packet format was returned as a return parameter (a
value outside supported range for exinf, wtsk, stsk, msgsz and/or
frbufsz on a requesting node) 

@subheading DESCRIPTION:
This system call refers to the state of the message buffer specified by
@code{mbfid}, and returns information of a task waiting to send a
message (@code{stsk}), the size of the next message to be received
(@code{msgsz}), the free buffer size (@code{frbufsz}), information of a
task waiting to receive a message (@code{wtsk}), and its extended
information (@code{exinf}).

@code{Wtsk} and @code{stsk} indicate whether or not there is a task
waiting for the message buffer in question.  If there is no waiting
task, @code{wtsk} and @code{stsk} are returned as @code{FALSE} = 0.  If
there is a waiting task, @code{wtsk} and @code{stsk} are returned as
values other than 0.

An @code{E_NOEXS} error will result if the message buffer specified to
@code{ref_mbf} does not exist.

The size of the message at the head of the message buffer (the next
message to be received) is returned to @code{msgsz}.  If there are no
messages on the message buffer, @code{msgsz} will be returned as
@code{FALSE} = 0.  A message whose size is zero cannot be sent.

At least one of @code{msgsz} = @code{FALSE} and @code{wtsk} =
@code{FALSE} is always true in this system call.

@code{Frbufsz} indicates the amount of free memory in the message
buffer.  This value can be used to know the total approximate size of
the messages which can be sent to the message buffer.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.
