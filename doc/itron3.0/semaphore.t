@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  This is the chapter from the RTEMS ITRON User's Guide that
@c  documents the services provided by the semaphore
@c  manager.
@c
@c  $Id$
@c

@chapter Semaphore Manager

@section Introduction

The semaphore manager provides functions to allocate, delete, and
control counting semaphores.  This manager is based on the 
ITRON 3.0 standard.

The services provided by the semaphore manager are:

@itemize @bullet
@item @code{cre_sem} - Create Semaphore
@item @code{del_sem} - Delete Semaphore
@item @code{sig_sem} - Signal Semaphore
@item @code{wai_sem} - Wait on Semaphore
@item @code{preq_sem} - Poll and Request Semaphore
@item @code{twai_sem} - Wait on Semaphore with Timeout
@item @code{ref_sem} - Reference Semaphore Status
@end itemize

@section Background

@subsection Theory

Semaphores are used for synchronization and mutual exclusion by indicating
the availability and number of resources.  The task (the task which is
returning resources) notifying other tasks of an event increases the number
of resources held by the semaphore by one.  The task (the task which
will obtain resources) waiting for the event decreases the number of
resources held by the semaphore by one.  If the number of resources held by a
semaphore is insufficient (namely 0), the task requiring resources will
wait until the next time resources are returned to the semaphore.  If there is
more than one task waiting for a semaphore, the tasks will be placed in the
queue.


@subsection T_CSEM Structure

The T_CSEM structure is used to define the characteristics of a semaphore
and passed an as argument to the @code{cre_sem} service.  The structure
is defined as follows:

@example
@group
/*
 *  Create Semaphore (cre_sem) Structure
 */

typedef struct t_csem @{
  VP    exinf;    /* extended information */
  ATR   sematr;   /* semaphore attributes */
  /* Following is the extended function for [level X]. */
  INT   isemcnt;   /* initial semaphore count */
  INT   maxsem;    /* maximum semaphore count */
  /* additional implementation dependent information may be included */
@} T_CSEM;

/*
 *  sematr - Semaphore Attribute Values
 */

#define TA_TFIFO   0x00   /* waiting tasks are handled by FIFO */
#define TA_TPRI    0x01   /* waiting tasks are handled by priority */

@end group
@end example

where the meaning of each field is:

@table @b
@item exinf
is for any extended information that the implementation may define.
This implementation does not use this field.

@item sematr
is the attributes for this semaphore.  The only attributed
which can be specified is whether tasks wait in FIFO (@code{TA_TFIFO})
or priority (@code{TA_TPRI}) order.

@item isemcnt
is the initial count of the semaphore.

@item maxsem
is the maximum count the semaphore may have.  It places an upper
limit on the value taken by the semaphore.

@end table

@subsection Building a Semaphore Attribute Set

In general, an attribute set is built by a bitwise OR
of the desired attribute components.  The following table lists
the set of valid semaphore attributes:

@itemize @bullet
@item @code{TA_TFIFO} - tasks wait by FIFO

@item @code{TA_TPRI} - tasks wait by priority

@end itemize

Attribute values are specifically designed to be
mutually exclusive, therefore bitwise OR and addition operations
are equivalent as long as each attribute appears exactly once in
the component list.

@subsection T_RSEM Structure

The T_RSEM structure is filled in by the @code{ref_sem} service with
status and state information on a semaphore.  The structure
is defined as follows:

@example
@group
/*
 *  Reference Semaphore (ref_sem) Structure
 */

typedef struct t_rsem @{
  VP      exinf;    /* extended information */
  BOOL_ID wtsk;     /* indicates whether there is a waiting task */
  INT     semcnt;   /* current semaphore count */
  /* additional implementation dependent information may be included */
@} T_RSEM;
@end group
@end example

@table @b

@item exinf
is for any extended information that the implementation may define.
This implementation does not use this field.

@item wtsk
is TRUE when there is one or more task waiting on the semaphore.
It is FALSE if no tasks are currently waiting.  The meaning of this
field is allowed to vary between ITRON implementations.  It may have
the ID of a waiting task, the number of tasks waiting, or a boolean
indication that one or more tasks are waiting.

@item semcnt
is the current semaphore count.

@end table

The information in this table is very volatile and should be used
with caution in an application.

@section Operations

@subsection Using as a Binary Semaphore

Creating a semaphore with a limit on the count of 1 effectively
restricts the semaphore to being a binary semaphore.  When the
binary semaphore is available, the count is 1.  When the binary
semaphore is unavailable, the count is 0.

Since this does not result in a true binary semaphore, advanced
binary features like the Priority Inheritance and Priority
Ceiling Protocols are not available.

@section System Calls

This section details the semaphore manager's services.
A subsection is dedicated to each of this manager's services
and describes the calling sequence, related constants, usage,
and status codes.


@c
@c  cre_sem
@c

@page
@subsection cre_sem - Create Semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER cre_sem(
  ID semid,
  T_CSEM *pk_csem
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOMEM} - Insufficient memory (Memory for control block cannot be
allocated)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.)

@code{E_RSATR} - Reserved attribute (sematr was invalid or could not be
used)

@code{E_OBJ} - Invalid object state (a semaphore of the same ID already
exists)

@code{E_PAR} - Parameter error (pk_csem is invalid and/or isemcnt or
maxsem is negative or invalid)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a task-
independent portion

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for exinf, sematr, isemcnt and/or
maxsem)

@subheading DESCRIPTION:

This routine creates a semaphore that resides on the local node.  The 
semaphore is initialized based on the attributes specified in the 
@code{pk_csem} structure.  The initial and maximum counts of the
semaphore are set based on the @code{isemcnt} and @code{maxsem} fields
in this structure.

Specifying @code{TA_TPRI} in the @code{sematr} field of the
semaphore attributes structure causes tasks
waiting for a semaphore to be serviced according to task
priority.  When @code{TA_TFIFO} is selected, tasks are serviced in First
In-First Out order.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

All memory is preallocated for RTEMS ITRON objects.  Thus, no dynamic
memory allocation is performed by @code{cre_sem} and the @code{E_NOMEM}
error can not be returned.

This directive will not cause the running task to be preempted.

The following semaphore attribute constants are
defined by RTEMS:

@itemize @bullet
@item @code{TA_TFIFO} - tasks wait by FIFO

@item @code{TA_TPRI} - tasks wait by priority

@end itemize

@c
@c  del_sem
@c

@page
@subsection del_sem - Delete Semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER del_sem(
  ID semid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a
task-independent portion

@subheading DESCRIPTION:

This routine deletes the semaphore specified by @code{semid}.
All tasks blocked waiting to acquire the semaphore will be
readied and returned a status code which indicates that the
semaphore was deleted.  The control block for this semaphore
is reclaimed by RTEMS.


@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

The calling task will be preempted if it is enabled
by the task's execution mode and a higher priority local task is
waiting on the deleted semaphore.  The calling task will NOT be
preempted if all of the tasks that are waiting on the semaphore
are remote tasks.

The calling task does not have to be the task that
created the semaphore.  Any local task that knows the semaphore
id can delete the semaphore.


@c
@c  sig_sem
@c

@page
@subsection sig_sem - Signal Semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER sig_sem(
  ID semid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_QOVR} - Queuing or nesting overflow (the queuing count given by
semcnt went over the maximum allowed)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_CTXID} - Specified an object on another node when the system call
was issued from a task in dispatch disabled state or from a
task-independent portion

@subheading DESCRIPTION:

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

@c
@c  wai_sem
@c

@page
@subsection wai_sem - Wait on Semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER wai_sem(
  ID semid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_DLT} - The object being waited for was deleted (the specified
semaphore was deleted while waiting)

@code{E_RLWAI} - Wait state was forcibly released (rel_wai was received
while waiting)

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for tmout)

@subheading DESCRIPTION:

This routine attempts to acquire the semaphore specified by @code{semid}.
If the semaphore is available (i.e. positive semaphore count), then the
semaphore count is decremented and the calling task returns immediately.  
Otherwise the calling tasking is blocked until the semaphore is released
by a subsequent invocation of @code{sig_sem}.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

If the semaphore is not available, then the calling task will be blocked.

@c
@c  preq_sem
@c

@page
@subsection preq_sem - Poll and Request Semaphore

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER preq_sem(
  ID semid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_TMOUT} - Polling failure or timeout exceeded

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for tmout)

@subheading DESCRIPTION:

This routine attempts to acquire the semaphore specified by @code{semid}.
If the semaphore is available (i.e. positive semaphore count), then the
semaphore count is decremented and the calling task returns immediately.
Otherwise, the @code{E_TMOUT} error is returned to the calling task to
indicate the semaphore is unavailable.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

This routine will not cause the running task to be preempted.

@c
@c  twai_sem
@c

@page
@subsection twai_sem - Wait on Semaphore with Timeout

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER twai_sem(
  ID semid,
  TMO tmout
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
specified from a user task.  This is implementation dependent.)

@code{E_PAR} - Parameter error (tmout is -2 or less)

@code{E_DLT} - The object being waited for was deleted (the specified
semaphore was deleted while waiting)

@code{E_RLWAI} - Wait state was forcibly released (rel_wai was received
while waiting)

@code{E_TMOUT} - Polling failure or timeout exceeded

@code{E_CTX} - Context error (issued from task-independent portions or a
task in dispatch disabled state)

@code{EN_OBJNO} - An object number which could not be accessed on the
target node is specified.

@code{EN_PAR} - A value outside the range supported by the target node
and/or transmission packet format was specified as a parameter (a value
outside supported range was specified for tmout)

@subheading DESCRIPTION:

This routine attempts to acquire the semaphore specified by @code{semid}.
If the semaphore is available (i.e. positive semaphore count), then the
semaphore count is decremented and the calling task returns immediately.
Otherwise the calling tasking is blocked until the semaphore is released
by a subsequent invocation of @code{sig_sem} or the timeout period specified
by @code{tmout} milliseconds is exceeded.  If the timeout period is exceeded,
then the @code{E_TMOUT} error is returned.

By specifiying @code{tmout} as @code{TMO_FEVR}, this routine has the same
behavior as @code{wai_sem}.  Similarly, by specifiying @code{tmout} as
@code{TMO_POL}, this routine has the same behavior as @code{preq_sem}.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

This routine may cause the calling task to block.

A clock tick is required to support the timeout functionality of
this routine.

@c
@c  ref_sem
@c

@page
@subsection ref_sem - Reference Semaphore Status

@subheading CALLING SEQUENCE:

@ifset is-C
@example
ER ref_sem(
  T_RSEM *pk_rsem,
  ID semid
);
@end example
@end ifset

@ifset is-Ada
@end ifset

@subheading STATUS CODES:

@code{E_OK} - Normal Completion

@code{E_ID} - Invalid ID number (semid was invalid or could not be used)

@code{E_NOEXS} - Object does not exist (the semaphore specified by semid
does not exist)

@code{E_OACV} - Object access violation (A semid less than -4 was
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
value outside supported range was specified for exinf, wtsk or semcnt)

@subheading DESCRIPTION:

This routine returns status information on the semaphore specified
by @code{semid}.  The @code{pk_rsem} structure is filled in by 
this service call.

@subheading NOTES:

Multiprocessing is not supported.  Thus none of the "EN_" status codes
will be returned.

This routine will not cause the running task to be preempted.

