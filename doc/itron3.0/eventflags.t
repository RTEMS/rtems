@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the eventflags
@c  manager.
@c
@c  $Id$
@c

@chapter Eventflags Manager

@section Introduction

The eventflag manager provides a high performance method of intertask communication and synchronization. The directives provided by the eventflag manager are:

The services provided by the eventflags manager are:

@itemize @bullet
@item @code{cre_flg} - Create Eventflag
@item @code{del_flg} - Delete Eventflag
@item @code{set_flg} - Set Eventflag
@item @code{clr_flg} - Clear Eventflag
@item @code{wai_flg} - Wait on Eventflag
@item @code{pol_flg} - Wait for Eventflag (Polling)
@item @code{twai_flg} - Wait on Eventflag with Timeout
@item @code{ref_flg} - Reference Eventflag Status
@end itemize

@section Background

@subsection Event sets

An eventflag is used by a task (or ISR) to inform another task of the
occurrence of a significant situation. One word bit-field is associated with each eventflags. The application developer should remember the
following key characteristics of event operations when utilizing the event
manager:

@itemize @bullet
@item Events provide a simple synchronization facility.
@item Events are aimed at tasks.
@item Tasks can wait on more than one event simultaneously.
@item Events are independent of one another.
@item Events do not hold or transport data.
@item Events are not queued. In other words, if an event is sent more than once to a task before being received, the second and subsequent send operations to that same task have no effect.
@end itemize

A pending event is an event that has been set. An
event condition is used to specify the events which the task desires to
receive and the algorithm which will be used to determine when the request
is satisfied. An event condition is satisfied based upon one of two
algorithms which are selected by the user. The @code{TWF_ORW} algorithm
states that an event condition is satisfied when at least a single
requested event is posted. The @code{TWF_ANDW} algorithm states that an
event condition is satisfied when every requested event is posted.

An eventflags or condition is built by a bitwise OR of the desired events.
If an event is not explicitly specified in the set or condition, then it
is not present. Events are specifically designed to be mutually exclusive,
therefore bitwise OR and addition operations are equivalent as long as
each event appears exactly once in the event set list.

@subsection T_CFLG Structure

The T_CFLG structire is used to define the characteristics of an eventflag
and passed as an argument to the @code{cre_flg} service. The structure is
defined as follows:

@example

/* 
 * Create Eventflags (cre_flg) Structure 
 */ 

typedef struct t_cflg @{
  VP exinf;     /* extended information */
  ATR flgatr;   /* eventflag attribute */
  UINT iflgptn; /* initial eventflag */
  /* additional implementation dependent information may be included */
@} T_CFLG; 

/* 
 *  flgatr - Eventflag Attribute Values
 */ 


/* multiple tasks are not allowed to wait (Wait Single Task)*/ 

#define TA_WSGL 0x00

/* multiple tasks are allowed to wait (Wait Multiple Task) */ 

#define TA_WMUL 0x08

/* wfmode */
#define TWF_ANDW 0x00 /* AND wait */ 
#define TWF_ORW 0x02 /* OR wait */ 
#define TWF_CLR 0x01 /* clear specification */ 
@end example

where the meaning of each field is:

@table @b

@item exinf

may be used freely by the user for including extended information about
the eventflag to be created. Information set here may be accessed by
@code{ref_flg}. If a larger region is desired for including user information, or
if the user wishes to change the contents of this information, the usr
should allocate memory area and set the address of this memory packet to
@code{exinf}.  The OS does not take care of the contents of @code{exinf}. This
implementation does not use this field.

@item flgatr

is the attributes for this eventflag. The lower bits of flgatr represent
system attributes, while the upper bits represent implementation-dependent
attributes.

@item iflgptn

is the initial eventflag pattern.
(CPU and/or implementation-dependent information may also be included)

@end table

@subsection T_RFLG Structure

The T_RFLG structire is used to define the characteristics of an eventflag
and passed as an argument to the @code{ref_flg} service. The structure is
defined as follows:

@example
/* Reference Eventflags (ref_flg) Structure */ 
typedef struct t_rflg @{
 VP       exinf;  /* extended information */
 BOOL_ID  wtsk;   /* indicates whether or not there is a waiting task */
 UINT     flgptn; /* eventflag bit pattern */
 /* additional implementation dependent information may be included */
@} T_RFLG; 
@end example

@table @b

@item exinf

see @code{T_CFLG}.

@item wtsk

indicates whether or not there is a task waiting for the eventflag in
question.  If there is no waiting task, @code{wtsk} is returned as FALSE = 0.  
If there is a waiting task, @code{wtsk} is returned as a value other than 0.

@item flgptn

is the eventflag pattern.

@end table

@section Operations

@section System Calls

This section details the eventflags manager's services. A subsection is
dedicated to each of this manager's services and describes the calling
sequence, related constants, usage, and status codes.


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

@code{E_OK}  -  Normal Completion

@code{E_NOMEM} - Insufficient memory (Memory for control block cannot be
allocated)

@code{E_ID} - Invalid ID number (flgid was invalid or could not be used)

@code{E_RSATR} - Reserved attribute (flgatr was invalid or could not be
used)

@code{E_OBJ} - Invalid object state (an eventflag of the same ID already
exists)

@code{E_OACV} - Object access violation (A flgid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (pk_cflg is invalid)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a
task-independent portion

@code{EN_PAR}- A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for exinf, flgatr and/or iflgptn)

@subheading DESCRIPTION:

This system call creates the eventflag specified by @code{flgid}.  
Specifically, a control block for the eventflag to be created is allocated
and the associated flag pattern is initialized using @code{iflgptn}.  A
single eventflag handles one word's worth of bits of the processor in
question as a group.  All operations are done in single word units.

User eventflags have positive ID numbers, while system eventflags have
negative ID numbers.  User tasks (tasks having positive task IDs) cannot
access system eventflags. An @code{E_OACV} error will result if a user
task issues a system call on a system eventflag, but error detection is
implementation dependent.

Eventflags having ID numbers from -4 through 0 cannot be created.  An
@code{E_ID} error will result if a value in this range is specified for
@code{flgid}.

The system attribute part of @code{flgatr} may be specified as @code{TA_WSGL} 
(Wait Single Task) or @code{TA_WMUL} (Wait Multiple Tasks)

@subheading NOTES:

Multiprocessing is not supported. Thus none of the "@code{EN_}" status
codes will be returned.

All memory is preallocated for @code{RTEMS ITRON} objects. Thus, no
dynamic memory allocation is performed by @code{cre_flg} and the
@code{E_NOMEM} error can not be returned.

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

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (flgid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the eventflag specified by flgid
does not exist)

@code{E_OACV} - Object access violation (A flgid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a
task-independent portion

@subheading DESCRIPTION:

This system call deletes the eventflag specified by @code{flgid}.

Issuing this system call causes memory used for the control block of the
associated eventflag to be released.  After this system call is invoked,
another eventflag having the same ID number can be created.

This system call will complete normally even if there are tasks waiting
for the eventflag.  In that case, an @code{E_DLT} error will be returned
to each waiting task.

@subheading NOTES:

Multiprocessing is not supported. Thus none of the "@code{EN_}" status
codes will be returned.

When an eventflag being waited for by more than one tasks is deleted, the
order of tasks on the ready queue after the WAIT state is cleared is
implementation dependent in the case of tasks having the same priority.

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

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (flgid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the eventflag specified by flgid
does not exist)

@code{E_OACV} - Object access violation (A flgid less than -4 was
specified from a user task. This is implementation dependent.)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a
task-independent portion

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for setptn or clrptn)

@subheading DESCRIPTION:

The @code{set_flg} system call sets the bits specified by @code{setptn} of the
one word eventflag specified by @code{flgid}.  In other words, a logical
sum is taken for the values of the eventflag specified by @code{flgid} with the
value of @code{setptn}.

If the eventflag value is changed by @code{set_flg} and the new eventflag
value satisfies the condition to release the WAIT state of the task which
issued @code{wai_flg} on the eventflag, the WAIT state of that task will
be released and the task will be put into RUN or READY state (or SUSPEND
state if the task was in WAIT-SUSPEND).

Nothing will happen to the target eventflag if all bits of @code{setptn}
are specified as 0 with @code{set_flg}. No error will result in either
case.

Multiple tasks can wait for a single eventflag if that eventflags has the
@code{TA_WMUL} attribute. This means that even eventflags can make queues
for tasks to wait on. When such eventflags are used, a single
@code{set_flg} call may result in the release of multiple waiting tasks.
In this case, the order of tasks on the ready queue after the WAIT state
is cleared is preserved for tasks having the same priority.

@subheading NOTES:

Multiprocessing is not supported. Thus none of the "@code{EN_}" status
codes will be returned.

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


@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (flgid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the eventflag specified by flgid
does not exist)

@code{E_OACV} - Object access violation (A flgid less than -4 was
specified from a user task. This is implementation dependent.)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a
task-independent portion

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for setptn or clrptn)

@subheading DESCRIPTION:

The @code{clr_flg} system call clears the bits of the one word eventflag
based on the corresponding zero bits of @code{clrptn}.  In other words, a
logical product is taken for the values of the eventflag specified by
@code{flgid} with the value of @code{clrptn}.

Issuing @code{clr_flg} never results in wait conditions being released on
a task waiting for the specified eventflag.  In other words, dispatching
never occurs with @code{clr_flg}.

Nothing will happen to the target eventflag if all bits of @code{clrptn}
are specified as 1 with @code{clr_flg}. No error will result.

@subheading NOTES:

Multiprocessing is not supported. Thus none of the "@code{EN_}" status
codes will be returned.

@c
@c wai_flg
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


@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (flgid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the eventflag specified by flgid
does not exist)

@code{E_OACV} - Object access violation (A flgid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (waiptn = 0, wfmode invalid, or tmout is -2
or less)

@code{E_OBJ} - Invalid object state (multiple tasks waiting for an
eventflag with the TA_WSGL attribute)

@code{E_DLT} - The object being waited for was deleted (the specified
eventflag was deleted while waiting)

@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received
while waiting)

@code{E_TMOUT} - Polling failure or timeout exceeded

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for waiptn and tmout)

@code{EN_RPAR} - A value outside the range supported by the requesting
node and/or transmission packet format was specified as a parameter (a
value exceeding the range for the requesting node was specified for
flgptn)

@subheading DESCRIPTION:

The @code{wai_flg} system call waits for the eventflag specified by
@code{flgid} to be set to satisfy the wait release condition specified by
@code{wfmode}. The Eventflags bit-pattern will be returned with a pointer @code{p_flgptn}.

If the eventflag specified by @code{flgid} already satisfies the wait
release conditions given by @code{wfmode}, the issuing task will continue
execution without waiting.  @code{wfmode} may be specified as follows.

@example
        wfmode = TWF_ANDW (or TWF_ORW) | TWF_CLR(optional)
@end example

If @code{TWF_ORW} is specified, the issuing task will wait for any of the
bits specified by @code{waiptn} to be set for the eventflag given by
@code{flgid} (OR wait).  If @code{TWF_ANDW} is specified, the issuing task
will wait for all of the bits specified by @code{waiptn} to be set for the
eventflag given by @code{flgid} (AND wait).

If the @code{TWF_CLR} specification is not present, the eventflag value
will remain unchanged even after the wait conditions have been satisfied
and the task has been released from the WAIT state.  If @code{TWF_CLR} is
specified, all bits of the eventflag will be cleared to 0 once the wait
conditions of the waiting task have been satisfied.

The return parameter @code{flgptn} returns the value of the eventflag after the
wait state of a task has been released due to this system call.  If
@code{TWF_CLR} was specified, the value before eventflag bits were cleared
is returned.  The value returned by @code{flgptn} fulfills the wait
release conditions of this system call.

An @code{E_PAR} parameter error will result if @code{waiptn} is 0.

A task can not execute any of @code{wai_flg, twai_flg} or @code{pol_flg}
on an eventflag having the @code{TA_WSGL} attribute if another task is
already waiting for that eventflag.  An @code{E_OBJ} error will be
returned to a task which executes @code{wai_flg} at a later time
regardless as to whether or not the task that executes @code{wai_flg} or
@code{twai_flg} later will be placed in a WAIT state (conditions for
releasing wait state be satisfied).  An @code{E_OBJ} error will be
returned even to tasks which just execute @code{pol_flg}, again regardless
as to whether or not wait release conditions for that task were satisfied.

On the other hand, multiple tasks can wait at the same time for the same
eventflag if that eventflag has the @code{TA_WMUL} attribute.  This means
that event flags can make queues for tasks to wait on.  When such
eventflags are used, a single @code{set_flg} call may release multiple
waiting tasks.

The following processing takes place if a queue for allowing multiple
tasks to wait has been created for an eventflag with the @code{TA_WMUL}
attribute.

@itemize @bullet

@item The waiting order on the queue is FIFO.  (However, depending on
@code{waiptn} and @code{wfmode}, task at the head of the queue will not
always be released from waiting.)

@item If a task specifying that the eventflag be cleared is on the queue,
the flag is cleared when that task is released from waiting.

@item Since any tasks behind a task which clears the eventflag (by
specifying @code{TWF_CLR}) will check the eventflag after it is cleared,
they will not be released from waiting.

@end itemize

If multiple tasks having the same priority are released from waiting
simultaneously due to @code{set_flg}, the order of tasks on the ready
queue after release will be the same as their original order on the
eventflag queue.

@subheading NOTES:
 
Multiprocessing is not supported. Thus none of the "@code{EN_}" status
codes will be returned.


@c
@c  pol_flg
@c

@page
@subsection pol_flg - Wait for Eventflag (Polling)

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


@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (flgid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the eventflag specified by flgid
does not exist)

@code{E_OACV} - Object access violation (A flgid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (waiptn = 0, wfmode invalid, or tmout is -2
or less)

@code{E_OBJ} - Invalid object state (multiple tasks waiting for an
eventflag with the TA_WSGL attribute)

@code{E_DLT} - The object being waited for was deleted (the specified
eventflag was deleted while waiting)

@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received
while waiting)

@code{E_TMOUT} - Polling failure or timeout exceeded

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for waiptn and tmout)

@code{EN_RPAR} - A value outside the range supported by the requesting
node and/or transmission packet format was specified as a parameter (a
value exceeding the range for the requesting node was specified for
flgptn)

@subheading DESCRIPTION:

The @code{pol_flg} system call has the same function as @code{wai_flg}
except for the waiting feature. @code{pol_flg} polls whether or not the
task should wait if @code{wai_flg} is executed. The meanings of
parameters to @code{pol_flg} are the same as for @code{wai_flg}.  The
specific operations by @code{pol_flg} are as follows.

@itemize @bullet

@item If the target eventflag already satisfies the conditions for
releasing wait given by @code{wfmode}, processing is the same as
@code{wai_flg}: the eventflag is cleared if @code{TWF_CLR} is specified
and the system call completes normally.

@item If the target eventflag does not yet satisfy the conditions for
releasing wait given by @code{wfmode}, an @code{E_TMOUT} error is returned to
indicate polling failed and the system call finishes. Unlike
@code{wai_flg}, the issuing task does not wait in this case. The eventflag
is not cleared in this case even if @code{TWF_CLR} has been specified.

@end itemize

@subheading NOTES:
 
Multiprocessing is not supported. Thus none of the "@code{EN_}" status
codes will be returned.


@c
@c twai_flg
@c

@page
@subsection twai_flg - Wait on Eventflag with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER twai_flg(
  UINT *p_flgptn,
  ID flgid,
  UINT waiptn,
  UINT wfmode,
  TMO tmout
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:


@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (flgid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the eventflag specified by flgid
does not exist)

@code{E_OACV} - Object access violation (A flgid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (waiptn = 0, wfmode invalid, or tmout is -2
or less)

@code{E_OBJ} - Invalid object state (multiple tasks waiting for an
eventflag with the TA_WSGL attribute)

@code{E_DLT} - The object being waited for was deleted (the specified
eventflag was deleted while waiting)

@code{E_RLWAI} - WAIT state was forcibly released (rel_wai was received
while waiting)

@code{E_TMOUT} - Polling failure or timeout exceeded

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for waiptn and tmout)

@code{EN_RPAR} - A value outside the range supported by the requesting
node and/or transmission packet format was specified as a parameter (a
value exceeding the range for the requesting node was specified for
flgptn)

@subheading DESCRIPTION:

The @code{twai_flg} system call has the same function as @code{wai_flg}
with an additional timeout feature.  A maximum wait time (timeout value)
can be specified using the parameter @code{tmout}. When a timeout is specified,
a timeout error, @code{E_TMOUT}, will result and the system call will
finish if the period specified by @code{tmout} elapses without conditions for
releasing wait being satisfied.

Specifying @code{TMO_POL = 0} to @code{twai_flg} for @code{tmout} indicates that
a timeout value of 0 be used, resulting in exactly the same processing as
@code{pol_flg}.  Specifying @code{TMO_FEVR = -1} to @code{twai_flg} for
@code{tmout} indicates that an infinite timeout value be used, resulting in
exactly the same processing as @code{wai_flg}.

@subheading NOTES:
 
Multiprocessing is not supported. Thus none of the "@code{EN_}" status
codes will be returned.


@code{Pol_flg} and @code{wai_flg} represent the same processing as
specifying certain values (@code{TMO_POL} or @code{TMO_FEVR}) to
@code{twai_flg} for tmout.  As such, only @code{twai_flg} is implemented
in the kernel; @code{pol_flg} and @code{wai_flg} should be implemented as macros
which call @code{twai_flg}.

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
  ID flgid );
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:


@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (flgid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the eventflag specified by flgid
does not exist)

@code{E_OACV} - Object access violation (A flgid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (the packet address for the return
parameters could not be used)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a
task-independent portion

@code{EN_RPAR} - A value outside the range supported by the requesting
node and/or transmission packet format was returned as a parameter (a
value outside supported range was specified for exinf, wtsk and/or flgptn)

@subheading DESCRIPTION:

This system call refers to the state of the eventflag specified by
@code{flgid}, and returns its current flag pattern (@code{flgptn}),
waiting task information (@code{wtsk}), and its extended information
(@code{exinf}).

Depending on the implementation, @code{wtsk} may be returned as the ID
(non-zero) of the task waiting for the eventflag.  If there are multiple
tasks waiting for the eventflag (only when attribute is @code{TA_WMUL}),
the ID of the task at the head of the queue is returned.

An @code{E_NOEXS} error will result if the eventflag specified to
@code{ref_flg} does not exist.

@subheading NOTES:

Multiprocessing is not supported. Thus none of the "@code{EN_}" status
codes will be returned.

Although both @code{ref_flg} and @code{pol_flg} can be used to find an
eventflag's pattern (@code{flgptn}) without causing the issuing task to
wait, @code{ref_flg} simply reads the eventflag's pattern (@code{flgptn})
while @code{pol_flg} functions is identical to @code{wai_flg} when wait
release conditions are satisfied (it clears the eventflag if
@code{TWF_CLR} is specified).

Depending on the implementation, additional information besides
@code{wtsk} and @code{flgptn} (such as eventflag attributes,
@code{flgatr}) may also be referred.
